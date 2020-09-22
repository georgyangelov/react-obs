import { create } from 'domain';
import { Socket } from 'net';
import { v4 as uuid } from 'uuid';
import { AppendChild, ApplyUpdate, ClientMessage, InitRequest, Prop, UpdateSource, RemoveChild, CreateSource, ObjectValue } from './generated/protocol_pb';
import { Instance, PropChanges, Props } from './types';

export class ServerAPI {
  private clientId: string = uuid();

  constructor(private socket: Socket) {
    this.socket.setEncoding('binary');
  }

  initialize() {
    console.debug(`Connected to server as ${this.clientId}`);

    const initRequest = new InitRequest();
    initRequest.setClientId(this.clientId);

    const packet = new ClientMessage();
    packet.setInitRequest(initRequest);

    this.send(packet);

    this.socket.on('readable', () => {
      if (this.socket.readableLength === 0) {
        console.log('Socket closed. Disconnected from server');
        return;
      }

      console.log('Stream is readable, received response');
    });
  }

  createSource(id: string, namePrefix: string, props: Props): Instance {
    const name = this.newName(namePrefix);

    const createSource = new CreateSource();
    createSource.setId(id);
    createSource.setName(name);
    createSource.setSettings(this.asObject(props));

    const applyUpdate = new ApplyUpdate();
    applyUpdate.setCreateSource(createSource);

    const message = new ClientMessage();
    message.setApplyUpdate(applyUpdate);

    this.send(message);

    return { name };
  }

  updateSource(source: Instance, propChanges: PropChanges) {
    const updateSource = new UpdateSource();
    updateSource.setName(source.name);
    updateSource.setChangedProps(this.asObject(propChanges));

    const applyUpdate = new ApplyUpdate();
    applyUpdate.setUpdateSource(updateSource);

    const message = new ClientMessage();
    message.setApplyUpdate(applyUpdate);

    this.send(message);
  }

  appendChild(parent: Instance, child: Instance) {
    const appendChild = new AppendChild();
    appendChild.setParentName(parent.name);
    appendChild.setChildName(child.name);

    const applyUpdate = new ApplyUpdate();
    applyUpdate.setAppendChild(appendChild);

    const message = new ClientMessage();
    message.setApplyUpdate(applyUpdate);

    this.send(message);
  }

  removeChild(parent: Instance, child: Instance) {
    const removeChild = new RemoveChild();
    removeChild.setParentName(parent.name);
    removeChild.setChildName(child.name);

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

  private newName(type: string) {
    return `${type}-${uuid()}`;
  }

  private send(message: ClientMessage) {
    const packetBinary = message.serializeBinary();

    const sizeHeader = new ArrayBuffer(4);
    const dataView = new DataView(sizeHeader);
    dataView.setUint32(0, packetBinary.byteLength, true);

    this.socket.write(new Uint8Array(sizeHeader));
    this.socket.write(packetBinary);
  }
};
