import { create } from 'domain';
import { Socket } from 'net';
import { v4 as uuid } from 'uuid';
import { AppendChild, ApplyUpdates, ClientMessage, CreateElement, ElementType, InitRequest, MapProp, PropValue, Update } from './generated/protocol_pb';
import { Instance, Props, Type } from './types';

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
      createElement.getPropsMap().set(key, this.valueAsProp(value));
    });

    const update = new Update();
    update.setCreateElement(createElement);

    const applyUpdates = new ApplyUpdates();
    applyUpdates.addUpdates(update)

    const message = new ClientMessage();
    message.setApplyUpdates(applyUpdates);

    this.send(message);

    return { name };
  }

  appendChild(parent: Instance, child: Instance) {
    const appendChild = new AppendChild();
    appendChild.setParentName(parent.name);
    appendChild.setChildName(child.name);

    const update = new Update();
    update.setAppendChild(appendChild);

    const applyUpdates = new ApplyUpdates();
    applyUpdates.addUpdates(update);

    const message = new ClientMessage();
    message.setApplyUpdates(applyUpdates);

    this.send(message);
  }

  private valueAsProp(value: string | boolean | number | object): PropValue {
    const propValue = new PropValue();

    if (typeof value === 'string') {
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
      const mapProp = new MapProp();

      Object.entries(value).forEach(([key, value]) => {
        mapProp.getMapMap().set(key, this.valueAsProp(value));
      });

      propValue.setMapValue(mapProp);
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
