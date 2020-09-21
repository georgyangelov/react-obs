const uuid = require('uuid').v4;
const protocol = require('./generated/protocol_pb');

module.exports = class ServerAPI {
  constructor(socket) {
    this.clientId = uuid();
    this.socket = socket;

    this.socket.setEncoding('binary');
  }

  initialize() {
    console.debug(`Connected to server as ${this.clientId}`);

    const initRequest = new protocol.InitRequest();
    initRequest.setClientId(this.clientId);

    const packet = new protocol.ClientMessage();
    packet.setInitRequest(initRequest);

    this._sendMessage(this.socket, packet);

    this.socket.on('readable', () => {
      if (this.socket.readableLength === 0) {
        console.log('Socket closed. Disconnected from server');
        return;
      }

      console.log('Stream is readable, received response');
    });
  }

  _sendMessage(socket, message) {
    const packetBinary = message.serializeBinary();

    const sizeHeader = new ArrayBuffer(4);
    const dataView = new DataView(sizeHeader);
    dataView.setUint32(0, packetBinary.byteLength, true);

    socket.write(new Uint8Array(sizeHeader));
    socket.write(packetBinary);
  }
};
