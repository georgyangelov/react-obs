const React = require('react');
const net = require('net');
const ServerAPI = require('./server-api');

console.log('Hello world');

const socket = net.createConnection({ port: 6666 }, () => {
  api.initialize();
});
const api = new ServerAPI(socket);
