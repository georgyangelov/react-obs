import { create } from 'domain';
import { Socket } from 'net';
import { v4 as uuid } from 'uuid';
import { AppendChild, ApplyUpdate, ClientMessage, CreateElement, ElementType, InitRequest, Prop, UpdateElement, Object as ObjectValue } from './generated/protocol_pb';
import { Instance, PropChange as PropChangeType, Props, Type } from './types';

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

  createElement(type: Type, props: Props): Instance {
    const name = this.newName(type);

    const createElement = new CreateElement();
    createElement.setName(name);

    switch (type) {
      case 'obs_text':
        createElement.setType(ElementType.TEXT);
        break;

      default:
        throw new Error(`Unsupported element type ${type}`);
    }

    Object.entries(props).forEach(([key, value]) => {
      createElement.addProps(this.asProp(key, value));
    });

    const applyUpdate = new ApplyUpdate();
    applyUpdate.setCreateElement(createElement);

    const message = new ClientMessage();
    message.setApplyUpdate(applyUpdate);

    this.send(message);

    return { name };
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

  updateProps(element: Instance, propChanges: PropChangeType[]) {
    const updateElement = new UpdateElement();
    updateElement.setName(element.name);

    propChanges.forEach(change => {
      updateElement.addChangedProps(this.asProp(change.key, change.value));
    });

    const applyUpdate = new ApplyUpdate();
    applyUpdate.setUpdateElement(updateElement);

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
      const objectValue = new ObjectValue();

      Object.entries(value).forEach(([key, value]) => {
        objectValue.addProps(this.asProp(key, value));
      });

      propValue.setObjectValue(objectValue);
    } else {
      throw new Error(`Unknown value type ${typeof value}`);
    }

    return propValue;
  }

  private newName(type: Type) {
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
