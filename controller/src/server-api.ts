import { BinaryReader } from 'google-protobuf';
import { Socket } from 'net';
import { v4 as uuid } from 'uuid';
import { AppendChild, ApplyUpdate, ClientMessage, InitRequest, Prop, UpdateSource, RemoveChild, CreateSource, ObjectValue, FindSourceRequest, Response, ServerMessage, CreateScene, CommitUpdates } from './generated/protocol_pb';
import { Container, Instance, PropChanges, Props } from './types';

class PacketReader {
  private readingPacket = false;
  private currentPacketSize = 0;
  private currentReadSize = 0;
  private buffer = Buffer.alloc(1 * 1024 * 1024);

  constructor(
    private socket: Socket,
    private onMessage: (message: ServerMessage) => void,
    private onDisconnect: () => void
  ) {}

  initialize() {
    this.socket.on('readable', () => {
      this.readSegment();
    });

    this.socket.on('close', () => {
      this.onDisconnect();
    });
  }

  private readPacketFromBuffer() {
    const reader = new BinaryReader(this.buffer, 0, this.currentReadSize);
    const message = new ServerMessage();

    ServerMessage.deserializeBinaryFromReader(message, reader);

    this.onMessage(message);
  }

  private readSegment() {
    if (this.socket.readableLength === 0) {
      return;
    }

    if (!this.readingPacket) {
      if (this.socket.readableLength < 4) {
        return;
      }

      const currentBuffer = Buffer.from(this.socket.read(4));
      this.currentPacketSize = currentBuffer.readUInt32BE(0);
      this.currentReadSize = 0;
      this.readingPacket = true;
    }

    const maxLengthToEndOfMessage = this.currentPacketSize - this.currentReadSize;
    const lengthToRead = Math.min(maxLengthToEndOfMessage, this.socket.readableLength);

    if (lengthToRead === 0) {
      return;
    }

    this.resizeBufferToFit(this.currentReadSize + lengthToRead);

    const currentBuffer = Buffer.from(this.socket.read(lengthToRead));
    currentBuffer.copy(this.buffer, this.currentReadSize);
    this.currentReadSize += lengthToRead;

    if (this.currentReadSize > this.currentPacketSize) {
      throw new Error('Error in readSegment(), currentReadSize must not be > currentPacketSize');
    }

    console.log('sizes', this.currentReadSize, this.currentPacketSize)

    if (this.currentReadSize === this.currentPacketSize) {
      this.readPacketFromBuffer();

      this.readingPacket = false;
      this.currentPacketSize = 0;
      this.currentReadSize = 0;

      this.readSegment();
    }
  }

  private resizeBufferToFit(requiredSize: number) {
    if (this.buffer.byteLength >= requiredSize) {
      return;
    }

    const newBufferSize = Math.max(requiredSize, this.buffer.byteLength * 2);
    const oldBuffer = this.buffer;

    this.buffer = Buffer.alloc(newBufferSize);
    oldBuffer.copy(this.buffer);
  }
}

export class ServerAPI {
  private clientId: string = uuid();
  private requests: Map<string, (error: any, response?: Response) => void> = new Map();
  private packetReader = new PacketReader(
    this.socket,
    message => this.messageReceived(message),
    () => this.disconnected()
  );

  constructor(private socket: Socket) {
    this.socket.setEncoding('binary');
  }

  async initialize(): Promise<void> {
    console.debug(`Connected to server as ${this.clientId}`);

    const initRequest = new InitRequest();
    initRequest.setRequestId(uuid());
    initRequest.setClientId(this.clientId);

    const packet = new ClientMessage();
    packet.setInitRequest(initRequest);

    this.packetReader.initialize();

    const response = await this.sendExpectingResponse(initRequest.getRequestId(), packet);

    console.log('Received response from server', response.toObject(false));
  }

  private messageReceived(message: ServerMessage) {
    console.debug('Received message from server', message.toObject(false));

    if (message.hasResponse()) {
      const response = message.getResponse()!;
      const request = this.requests.get(response.getRequestId());

      if (!request) {
        throw new Error('Received response but request cannot be found');
      }

      if (response.getSuccess()) {
        request(undefined, response);
      } else {
        request(new Error('Unsuccessful operation'), undefined);
      }
    }
  }

  private disconnected() {
    console.log('Disconnected from server');
  }

  async findUnmanagedSource(name: string): Promise<Container> {
    const uid = uuid();

    const findSourceRequest = new FindSourceRequest();
    findSourceRequest.setRequestId(uid);
    findSourceRequest.setUid(uid);
    findSourceRequest.setName(name);

    const packet = new ClientMessage();
    packet.setFindSource(findSourceRequest);

    await this.sendExpectingResponse(uid, packet);

    return { uid, unmanaged: true };
  }

  commitUpdates(containerUid: string) {
    const commitUpdates = new CommitUpdates();
    commitUpdates.setContainerUid(containerUid);

    const applyUpdate = new ApplyUpdate();
    applyUpdate.setCommitUpdates(commitUpdates);

    const message = new ClientMessage();
    message.setApplyUpdate(applyUpdate);

    this.send(message);
  }

  createSource(id: string, name: string, props: Props): Instance {
    const uid = uuid();

    const createSource = new CreateSource();
    createSource.setUid(uid);
    createSource.setId(id);
    createSource.setName(name);
    createSource.setSettings(this.asObject(props));

    const applyUpdate = new ApplyUpdate();
    applyUpdate.setCreateSource(createSource);

    const message = new ClientMessage();
    message.setApplyUpdate(applyUpdate);

    this.send(message);

    return { uid };
  }

  updateSource(source: Instance, propChanges: PropChanges) {
    const updateSource = new UpdateSource();
    updateSource.setUid(source.uid);
    updateSource.setChangedProps(this.asObject(propChanges));

    const applyUpdate = new ApplyUpdate();
    applyUpdate.setUpdateSource(updateSource);

    const message = new ClientMessage();
    message.setApplyUpdate(applyUpdate);

    this.send(message);
  }

  createScene(name: string): Instance {
    const uid = uuid();

    const createScene = new CreateScene();
    createScene.setUid(uid);
    createScene.setName(name);

    const applyUpdate = new ApplyUpdate();
    applyUpdate.setCreateScene(createScene);

    const message = new ClientMessage();
    message.setApplyUpdate(applyUpdate);

    this.send(message);

    return { uid };
  }

  appendChild(parent: Instance, child: Instance) {
    const appendChild = new AppendChild();
    appendChild.setParentUid(parent.uid);
    appendChild.setChildUid(child.uid);

    const applyUpdate = new ApplyUpdate();
    applyUpdate.setAppendChild(appendChild);

    const message = new ClientMessage();
    message.setApplyUpdate(applyUpdate);

    this.send(message);
  }

  removeChild(parent: Instance, child: Instance) {
    const removeChild = new RemoveChild();
    removeChild.setParentUid(parent.uid);
    removeChild.setChildUid(child.uid);

    const applyUpdate = new ApplyUpdate();
    applyUpdate.setRemoveChild(removeChild);

    const message = new ClientMessage();
    message.setApplyUpdate(applyUpdate);

    this.send(message);
  }

  private asProp(key: string, value: string | boolean | number | object | undefined): Prop {
    const propValue = new Prop();
    propValue.setKey(key);

    if (typeof value === 'undefined') {
      propValue.setUndefined(true);
    } else if (typeof value === 'string') {
      propValue.setStringValue(value);
    } else if (typeof value === 'boolean') {
      propValue.setBoolValue(value);
    } else if (typeof value === 'number') {
      if (Number.isInteger(value)) {
        propValue.setIntValue(value);
      } else {
        propValue.setFloatValue(value);
      }
    } else if (typeof value === 'object') {
      propValue.setObjectValue(this.asObject(value));
    } else {
      throw new Error(`Unknown value type ${typeof value}`);
    }

    return propValue;
  }

  private asObject(value: object): ObjectValue {
    const objectValue = new ObjectValue();

    Object.entries(value).forEach(([key, value]) => {
      objectValue.addProps(this.asProp(key, value));
    });

    return objectValue;
  }

  private send(message: ClientMessage) {
    const packetBinary = message.serializeBinary();

    const sizeHeader = new ArrayBuffer(4);
    const dataView = new DataView(sizeHeader);
    dataView.setUint32(0, packetBinary.byteLength);

    this.socket.write(new Uint8Array(sizeHeader));
    this.socket.write(packetBinary);
  }

  private sendExpectingResponse(requestId: string, packet: ClientMessage): Promise<Response> {
    return new Promise((resolve, reject) => {
      this.requests.set(requestId, (error, response) => {
        if (error) {
          reject(error);
          return;
        }

        resolve(response);
      });

      this.send(packet);
    });
  }
};
