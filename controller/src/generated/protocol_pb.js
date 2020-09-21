// source: protocol.proto
/**
 * @fileoverview
 * @enhanceable
 * @suppress {messageConventions} JS Compiler reports an error if a variable or
 *     field starts with 'MSG_' and isn't a translatable message.
 * @public
 */
// GENERATED CODE -- DO NOT EDIT!

var jspb = require('google-protobuf');
var goog = jspb;
var global = Function('return this')();

goog.exportSymbol('proto.protocol.ClientMessage', null, global);
goog.exportSymbol('proto.protocol.ClientMessage.MessageCase', null, global);
goog.exportSymbol('proto.protocol.InitRequest', null, global);
goog.exportSymbol('proto.protocol.InitResponse', null, global);
goog.exportSymbol('proto.protocol.Scene', null, global);
goog.exportSymbol('proto.protocol.ServerMessage', null, global);
goog.exportSymbol('proto.protocol.ServerMessage.MessageCase', null, global);
/**
 * Generated by JsPbCodeGenerator.
 * @param {Array=} opt_data Optional initial data array, typically from a
 * server response, or constructed directly in Javascript. The array is used
 * in place and becomes part of the constructed object. It is not cloned.
 * If no data is provided, the constructed object will be empty, but still
 * valid.
 * @extends {jspb.Message}
 * @constructor
 */
proto.protocol.ClientMessage = function(opt_data) {
  jspb.Message.initialize(this, opt_data, 0, -1, null, proto.protocol.ClientMessage.oneofGroups_);
};
goog.inherits(proto.protocol.ClientMessage, jspb.Message);
if (goog.DEBUG && !COMPILED) {
  /**
   * @public
   * @override
   */
  proto.protocol.ClientMessage.displayName = 'proto.protocol.ClientMessage';
}
/**
 * Generated by JsPbCodeGenerator.
 * @param {Array=} opt_data Optional initial data array, typically from a
 * server response, or constructed directly in Javascript. The array is used
 * in place and becomes part of the constructed object. It is not cloned.
 * If no data is provided, the constructed object will be empty, but still
 * valid.
 * @extends {jspb.Message}
 * @constructor
 */
proto.protocol.ServerMessage = function(opt_data) {
  jspb.Message.initialize(this, opt_data, 0, -1, null, proto.protocol.ServerMessage.oneofGroups_);
};
goog.inherits(proto.protocol.ServerMessage, jspb.Message);
if (goog.DEBUG && !COMPILED) {
  /**
   * @public
   * @override
   */
  proto.protocol.ServerMessage.displayName = 'proto.protocol.ServerMessage';
}
/**
 * Generated by JsPbCodeGenerator.
 * @param {Array=} opt_data Optional initial data array, typically from a
 * server response, or constructed directly in Javascript. The array is used
 * in place and becomes part of the constructed object. It is not cloned.
 * If no data is provided, the constructed object will be empty, but still
 * valid.
 * @extends {jspb.Message}
 * @constructor
 */
proto.protocol.InitRequest = function(opt_data) {
  jspb.Message.initialize(this, opt_data, 0, -1, null, null);
};
goog.inherits(proto.protocol.InitRequest, jspb.Message);
if (goog.DEBUG && !COMPILED) {
  /**
   * @public
   * @override
   */
  proto.protocol.InitRequest.displayName = 'proto.protocol.InitRequest';
}
/**
 * Generated by JsPbCodeGenerator.
 * @param {Array=} opt_data Optional initial data array, typically from a
 * server response, or constructed directly in Javascript. The array is used
 * in place and becomes part of the constructed object. It is not cloned.
 * If no data is provided, the constructed object will be empty, but still
 * valid.
 * @extends {jspb.Message}
 * @constructor
 */
proto.protocol.InitResponse = function(opt_data) {
  jspb.Message.initialize(this, opt_data, 0, -1, proto.protocol.InitResponse.repeatedFields_, null);
};
goog.inherits(proto.protocol.InitResponse, jspb.Message);
if (goog.DEBUG && !COMPILED) {
  /**
   * @public
   * @override
   */
  proto.protocol.InitResponse.displayName = 'proto.protocol.InitResponse';
}
/**
 * Generated by JsPbCodeGenerator.
 * @param {Array=} opt_data Optional initial data array, typically from a
 * server response, or constructed directly in Javascript. The array is used
 * in place and becomes part of the constructed object. It is not cloned.
 * If no data is provided, the constructed object will be empty, but still
 * valid.
 * @extends {jspb.Message}
 * @constructor
 */
proto.protocol.Scene = function(opt_data) {
  jspb.Message.initialize(this, opt_data, 0, -1, null, null);
};
goog.inherits(proto.protocol.Scene, jspb.Message);
if (goog.DEBUG && !COMPILED) {
  /**
   * @public
   * @override
   */
  proto.protocol.Scene.displayName = 'proto.protocol.Scene';
}

/**
 * Oneof group definitions for this message. Each group defines the field
 * numbers belonging to that group. When of these fields' value is set, all
 * other fields in the group are cleared. During deserialization, if multiple
 * fields are encountered for a group, only the last value seen will be kept.
 * @private {!Array<!Array<number>>}
 * @const
 */
proto.protocol.ClientMessage.oneofGroups_ = [[1]];

/**
 * @enum {number}
 */
proto.protocol.ClientMessage.MessageCase = {
  MESSAGE_NOT_SET: 0,
  INIT_REQUEST: 1
};

/**
 * @return {proto.protocol.ClientMessage.MessageCase}
 */
proto.protocol.ClientMessage.prototype.getMessageCase = function() {
  return /** @type {proto.protocol.ClientMessage.MessageCase} */(jspb.Message.computeOneofCase(this, proto.protocol.ClientMessage.oneofGroups_[0]));
};



if (jspb.Message.GENERATE_TO_OBJECT) {
/**
 * Creates an object representation of this proto.
 * Field names that are reserved in JavaScript and will be renamed to pb_name.
 * Optional fields that are not set will be set to undefined.
 * To access a reserved field use, foo.pb_<name>, eg, foo.pb_default.
 * For the list of reserved names please see:
 *     net/proto2/compiler/js/internal/generator.cc#kKeyword.
 * @param {boolean=} opt_includeInstance Deprecated. whether to include the
 *     JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @return {!Object}
 */
proto.protocol.ClientMessage.prototype.toObject = function(opt_includeInstance) {
  return proto.protocol.ClientMessage.toObject(opt_includeInstance, this);
};


/**
 * Static version of the {@see toObject} method.
 * @param {boolean|undefined} includeInstance Deprecated. Whether to include
 *     the JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @param {!proto.protocol.ClientMessage} msg The msg instance to transform.
 * @return {!Object}
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.protocol.ClientMessage.toObject = function(includeInstance, msg) {
  var f, obj = {
    initRequest: (f = msg.getInitRequest()) && proto.protocol.InitRequest.toObject(includeInstance, f)
  };

  if (includeInstance) {
    obj.$jspbMessageInstance = msg;
  }
  return obj;
};
}


/**
 * Deserializes binary data (in protobuf wire format).
 * @param {jspb.ByteSource} bytes The bytes to deserialize.
 * @return {!proto.protocol.ClientMessage}
 */
proto.protocol.ClientMessage.deserializeBinary = function(bytes) {
  var reader = new jspb.BinaryReader(bytes);
  var msg = new proto.protocol.ClientMessage;
  return proto.protocol.ClientMessage.deserializeBinaryFromReader(msg, reader);
};


/**
 * Deserializes binary data (in protobuf wire format) from the
 * given reader into the given message object.
 * @param {!proto.protocol.ClientMessage} msg The message object to deserialize into.
 * @param {!jspb.BinaryReader} reader The BinaryReader to use.
 * @return {!proto.protocol.ClientMessage}
 */
proto.protocol.ClientMessage.deserializeBinaryFromReader = function(msg, reader) {
  while (reader.nextField()) {
    if (reader.isEndGroup()) {
      break;
    }
    var field = reader.getFieldNumber();
    switch (field) {
    case 1:
      var value = new proto.protocol.InitRequest;
      reader.readMessage(value,proto.protocol.InitRequest.deserializeBinaryFromReader);
      msg.setInitRequest(value);
      break;
    default:
      reader.skipField();
      break;
    }
  }
  return msg;
};


/**
 * Serializes the message to binary data (in protobuf wire format).
 * @return {!Uint8Array}
 */
proto.protocol.ClientMessage.prototype.serializeBinary = function() {
  var writer = new jspb.BinaryWriter();
  proto.protocol.ClientMessage.serializeBinaryToWriter(this, writer);
  return writer.getResultBuffer();
};


/**
 * Serializes the given message to binary data (in protobuf wire
 * format), writing to the given BinaryWriter.
 * @param {!proto.protocol.ClientMessage} message
 * @param {!jspb.BinaryWriter} writer
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.protocol.ClientMessage.serializeBinaryToWriter = function(message, writer) {
  var f = undefined;
  f = message.getInitRequest();
  if (f != null) {
    writer.writeMessage(
      1,
      f,
      proto.protocol.InitRequest.serializeBinaryToWriter
    );
  }
};


/**
 * optional InitRequest init_request = 1;
 * @return {?proto.protocol.InitRequest}
 */
proto.protocol.ClientMessage.prototype.getInitRequest = function() {
  return /** @type{?proto.protocol.InitRequest} */ (
    jspb.Message.getWrapperField(this, proto.protocol.InitRequest, 1));
};


/**
 * @param {?proto.protocol.InitRequest|undefined} value
 * @return {!proto.protocol.ClientMessage} returns this
*/
proto.protocol.ClientMessage.prototype.setInitRequest = function(value) {
  return jspb.Message.setOneofWrapperField(this, 1, proto.protocol.ClientMessage.oneofGroups_[0], value);
};


/**
 * Clears the message field making it undefined.
 * @return {!proto.protocol.ClientMessage} returns this
 */
proto.protocol.ClientMessage.prototype.clearInitRequest = function() {
  return this.setInitRequest(undefined);
};


/**
 * Returns whether this field is set.
 * @return {boolean}
 */
proto.protocol.ClientMessage.prototype.hasInitRequest = function() {
  return jspb.Message.getField(this, 1) != null;
};



/**
 * Oneof group definitions for this message. Each group defines the field
 * numbers belonging to that group. When of these fields' value is set, all
 * other fields in the group are cleared. During deserialization, if multiple
 * fields are encountered for a group, only the last value seen will be kept.
 * @private {!Array<!Array<number>>}
 * @const
 */
proto.protocol.ServerMessage.oneofGroups_ = [[1]];

/**
 * @enum {number}
 */
proto.protocol.ServerMessage.MessageCase = {
  MESSAGE_NOT_SET: 0,
  INIT_RESPONSE: 1
};

/**
 * @return {proto.protocol.ServerMessage.MessageCase}
 */
proto.protocol.ServerMessage.prototype.getMessageCase = function() {
  return /** @type {proto.protocol.ServerMessage.MessageCase} */(jspb.Message.computeOneofCase(this, proto.protocol.ServerMessage.oneofGroups_[0]));
};



if (jspb.Message.GENERATE_TO_OBJECT) {
/**
 * Creates an object representation of this proto.
 * Field names that are reserved in JavaScript and will be renamed to pb_name.
 * Optional fields that are not set will be set to undefined.
 * To access a reserved field use, foo.pb_<name>, eg, foo.pb_default.
 * For the list of reserved names please see:
 *     net/proto2/compiler/js/internal/generator.cc#kKeyword.
 * @param {boolean=} opt_includeInstance Deprecated. whether to include the
 *     JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @return {!Object}
 */
proto.protocol.ServerMessage.prototype.toObject = function(opt_includeInstance) {
  return proto.protocol.ServerMessage.toObject(opt_includeInstance, this);
};


/**
 * Static version of the {@see toObject} method.
 * @param {boolean|undefined} includeInstance Deprecated. Whether to include
 *     the JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @param {!proto.protocol.ServerMessage} msg The msg instance to transform.
 * @return {!Object}
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.protocol.ServerMessage.toObject = function(includeInstance, msg) {
  var f, obj = {
    initResponse: (f = msg.getInitResponse()) && proto.protocol.InitResponse.toObject(includeInstance, f)
  };

  if (includeInstance) {
    obj.$jspbMessageInstance = msg;
  }
  return obj;
};
}


/**
 * Deserializes binary data (in protobuf wire format).
 * @param {jspb.ByteSource} bytes The bytes to deserialize.
 * @return {!proto.protocol.ServerMessage}
 */
proto.protocol.ServerMessage.deserializeBinary = function(bytes) {
  var reader = new jspb.BinaryReader(bytes);
  var msg = new proto.protocol.ServerMessage;
  return proto.protocol.ServerMessage.deserializeBinaryFromReader(msg, reader);
};


/**
 * Deserializes binary data (in protobuf wire format) from the
 * given reader into the given message object.
 * @param {!proto.protocol.ServerMessage} msg The message object to deserialize into.
 * @param {!jspb.BinaryReader} reader The BinaryReader to use.
 * @return {!proto.protocol.ServerMessage}
 */
proto.protocol.ServerMessage.deserializeBinaryFromReader = function(msg, reader) {
  while (reader.nextField()) {
    if (reader.isEndGroup()) {
      break;
    }
    var field = reader.getFieldNumber();
    switch (field) {
    case 1:
      var value = new proto.protocol.InitResponse;
      reader.readMessage(value,proto.protocol.InitResponse.deserializeBinaryFromReader);
      msg.setInitResponse(value);
      break;
    default:
      reader.skipField();
      break;
    }
  }
  return msg;
};


/**
 * Serializes the message to binary data (in protobuf wire format).
 * @return {!Uint8Array}
 */
proto.protocol.ServerMessage.prototype.serializeBinary = function() {
  var writer = new jspb.BinaryWriter();
  proto.protocol.ServerMessage.serializeBinaryToWriter(this, writer);
  return writer.getResultBuffer();
};


/**
 * Serializes the given message to binary data (in protobuf wire
 * format), writing to the given BinaryWriter.
 * @param {!proto.protocol.ServerMessage} message
 * @param {!jspb.BinaryWriter} writer
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.protocol.ServerMessage.serializeBinaryToWriter = function(message, writer) {
  var f = undefined;
  f = message.getInitResponse();
  if (f != null) {
    writer.writeMessage(
      1,
      f,
      proto.protocol.InitResponse.serializeBinaryToWriter
    );
  }
};


/**
 * optional InitResponse init_response = 1;
 * @return {?proto.protocol.InitResponse}
 */
proto.protocol.ServerMessage.prototype.getInitResponse = function() {
  return /** @type{?proto.protocol.InitResponse} */ (
    jspb.Message.getWrapperField(this, proto.protocol.InitResponse, 1));
};


/**
 * @param {?proto.protocol.InitResponse|undefined} value
 * @return {!proto.protocol.ServerMessage} returns this
*/
proto.protocol.ServerMessage.prototype.setInitResponse = function(value) {
  return jspb.Message.setOneofWrapperField(this, 1, proto.protocol.ServerMessage.oneofGroups_[0], value);
};


/**
 * Clears the message field making it undefined.
 * @return {!proto.protocol.ServerMessage} returns this
 */
proto.protocol.ServerMessage.prototype.clearInitResponse = function() {
  return this.setInitResponse(undefined);
};


/**
 * Returns whether this field is set.
 * @return {boolean}
 */
proto.protocol.ServerMessage.prototype.hasInitResponse = function() {
  return jspb.Message.getField(this, 1) != null;
};





if (jspb.Message.GENERATE_TO_OBJECT) {
/**
 * Creates an object representation of this proto.
 * Field names that are reserved in JavaScript and will be renamed to pb_name.
 * Optional fields that are not set will be set to undefined.
 * To access a reserved field use, foo.pb_<name>, eg, foo.pb_default.
 * For the list of reserved names please see:
 *     net/proto2/compiler/js/internal/generator.cc#kKeyword.
 * @param {boolean=} opt_includeInstance Deprecated. whether to include the
 *     JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @return {!Object}
 */
proto.protocol.InitRequest.prototype.toObject = function(opt_includeInstance) {
  return proto.protocol.InitRequest.toObject(opt_includeInstance, this);
};


/**
 * Static version of the {@see toObject} method.
 * @param {boolean|undefined} includeInstance Deprecated. Whether to include
 *     the JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @param {!proto.protocol.InitRequest} msg The msg instance to transform.
 * @return {!Object}
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.protocol.InitRequest.toObject = function(includeInstance, msg) {
  var f, obj = {
    clientId: jspb.Message.getFieldWithDefault(msg, 1, "")
  };

  if (includeInstance) {
    obj.$jspbMessageInstance = msg;
  }
  return obj;
};
}


/**
 * Deserializes binary data (in protobuf wire format).
 * @param {jspb.ByteSource} bytes The bytes to deserialize.
 * @return {!proto.protocol.InitRequest}
 */
proto.protocol.InitRequest.deserializeBinary = function(bytes) {
  var reader = new jspb.BinaryReader(bytes);
  var msg = new proto.protocol.InitRequest;
  return proto.protocol.InitRequest.deserializeBinaryFromReader(msg, reader);
};


/**
 * Deserializes binary data (in protobuf wire format) from the
 * given reader into the given message object.
 * @param {!proto.protocol.InitRequest} msg The message object to deserialize into.
 * @param {!jspb.BinaryReader} reader The BinaryReader to use.
 * @return {!proto.protocol.InitRequest}
 */
proto.protocol.InitRequest.deserializeBinaryFromReader = function(msg, reader) {
  while (reader.nextField()) {
    if (reader.isEndGroup()) {
      break;
    }
    var field = reader.getFieldNumber();
    switch (field) {
    case 1:
      var value = /** @type {string} */ (reader.readString());
      msg.setClientId(value);
      break;
    default:
      reader.skipField();
      break;
    }
  }
  return msg;
};


/**
 * Serializes the message to binary data (in protobuf wire format).
 * @return {!Uint8Array}
 */
proto.protocol.InitRequest.prototype.serializeBinary = function() {
  var writer = new jspb.BinaryWriter();
  proto.protocol.InitRequest.serializeBinaryToWriter(this, writer);
  return writer.getResultBuffer();
};


/**
 * Serializes the given message to binary data (in protobuf wire
 * format), writing to the given BinaryWriter.
 * @param {!proto.protocol.InitRequest} message
 * @param {!jspb.BinaryWriter} writer
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.protocol.InitRequest.serializeBinaryToWriter = function(message, writer) {
  var f = undefined;
  f = message.getClientId();
  if (f.length > 0) {
    writer.writeString(
      1,
      f
    );
  }
};


/**
 * optional string client_id = 1;
 * @return {string}
 */
proto.protocol.InitRequest.prototype.getClientId = function() {
  return /** @type {string} */ (jspb.Message.getFieldWithDefault(this, 1, ""));
};


/**
 * @param {string} value
 * @return {!proto.protocol.InitRequest} returns this
 */
proto.protocol.InitRequest.prototype.setClientId = function(value) {
  return jspb.Message.setProto3StringField(this, 1, value);
};



/**
 * List of repeated fields within this message type.
 * @private {!Array<number>}
 * @const
 */
proto.protocol.InitResponse.repeatedFields_ = [3];



if (jspb.Message.GENERATE_TO_OBJECT) {
/**
 * Creates an object representation of this proto.
 * Field names that are reserved in JavaScript and will be renamed to pb_name.
 * Optional fields that are not set will be set to undefined.
 * To access a reserved field use, foo.pb_<name>, eg, foo.pb_default.
 * For the list of reserved names please see:
 *     net/proto2/compiler/js/internal/generator.cc#kKeyword.
 * @param {boolean=} opt_includeInstance Deprecated. whether to include the
 *     JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @return {!Object}
 */
proto.protocol.InitResponse.prototype.toObject = function(opt_includeInstance) {
  return proto.protocol.InitResponse.toObject(opt_includeInstance, this);
};


/**
 * Static version of the {@see toObject} method.
 * @param {boolean|undefined} includeInstance Deprecated. Whether to include
 *     the JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @param {!proto.protocol.InitResponse} msg The msg instance to transform.
 * @return {!Object}
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.protocol.InitResponse.toObject = function(includeInstance, msg) {
  var f, obj = {
    canvasWidth: jspb.Message.getFieldWithDefault(msg, 1, 0),
    canvasHeight: jspb.Message.getFieldWithDefault(msg, 2, 0),
    scenesList: jspb.Message.toObjectList(msg.getScenesList(),
    proto.protocol.Scene.toObject, includeInstance)
  };

  if (includeInstance) {
    obj.$jspbMessageInstance = msg;
  }
  return obj;
};
}


/**
 * Deserializes binary data (in protobuf wire format).
 * @param {jspb.ByteSource} bytes The bytes to deserialize.
 * @return {!proto.protocol.InitResponse}
 */
proto.protocol.InitResponse.deserializeBinary = function(bytes) {
  var reader = new jspb.BinaryReader(bytes);
  var msg = new proto.protocol.InitResponse;
  return proto.protocol.InitResponse.deserializeBinaryFromReader(msg, reader);
};


/**
 * Deserializes binary data (in protobuf wire format) from the
 * given reader into the given message object.
 * @param {!proto.protocol.InitResponse} msg The message object to deserialize into.
 * @param {!jspb.BinaryReader} reader The BinaryReader to use.
 * @return {!proto.protocol.InitResponse}
 */
proto.protocol.InitResponse.deserializeBinaryFromReader = function(msg, reader) {
  while (reader.nextField()) {
    if (reader.isEndGroup()) {
      break;
    }
    var field = reader.getFieldNumber();
    switch (field) {
    case 1:
      var value = /** @type {number} */ (reader.readUint32());
      msg.setCanvasWidth(value);
      break;
    case 2:
      var value = /** @type {number} */ (reader.readUint32());
      msg.setCanvasHeight(value);
      break;
    case 3:
      var value = new proto.protocol.Scene;
      reader.readMessage(value,proto.protocol.Scene.deserializeBinaryFromReader);
      msg.addScenes(value);
      break;
    default:
      reader.skipField();
      break;
    }
  }
  return msg;
};


/**
 * Serializes the message to binary data (in protobuf wire format).
 * @return {!Uint8Array}
 */
proto.protocol.InitResponse.prototype.serializeBinary = function() {
  var writer = new jspb.BinaryWriter();
  proto.protocol.InitResponse.serializeBinaryToWriter(this, writer);
  return writer.getResultBuffer();
};


/**
 * Serializes the given message to binary data (in protobuf wire
 * format), writing to the given BinaryWriter.
 * @param {!proto.protocol.InitResponse} message
 * @param {!jspb.BinaryWriter} writer
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.protocol.InitResponse.serializeBinaryToWriter = function(message, writer) {
  var f = undefined;
  f = message.getCanvasWidth();
  if (f !== 0) {
    writer.writeUint32(
      1,
      f
    );
  }
  f = message.getCanvasHeight();
  if (f !== 0) {
    writer.writeUint32(
      2,
      f
    );
  }
  f = message.getScenesList();
  if (f.length > 0) {
    writer.writeRepeatedMessage(
      3,
      f,
      proto.protocol.Scene.serializeBinaryToWriter
    );
  }
};


/**
 * optional uint32 canvas_width = 1;
 * @return {number}
 */
proto.protocol.InitResponse.prototype.getCanvasWidth = function() {
  return /** @type {number} */ (jspb.Message.getFieldWithDefault(this, 1, 0));
};


/**
 * @param {number} value
 * @return {!proto.protocol.InitResponse} returns this
 */
proto.protocol.InitResponse.prototype.setCanvasWidth = function(value) {
  return jspb.Message.setProto3IntField(this, 1, value);
};


/**
 * optional uint32 canvas_height = 2;
 * @return {number}
 */
proto.protocol.InitResponse.prototype.getCanvasHeight = function() {
  return /** @type {number} */ (jspb.Message.getFieldWithDefault(this, 2, 0));
};


/**
 * @param {number} value
 * @return {!proto.protocol.InitResponse} returns this
 */
proto.protocol.InitResponse.prototype.setCanvasHeight = function(value) {
  return jspb.Message.setProto3IntField(this, 2, value);
};


/**
 * repeated Scene scenes = 3;
 * @return {!Array<!proto.protocol.Scene>}
 */
proto.protocol.InitResponse.prototype.getScenesList = function() {
  return /** @type{!Array<!proto.protocol.Scene>} */ (
    jspb.Message.getRepeatedWrapperField(this, proto.protocol.Scene, 3));
};


/**
 * @param {!Array<!proto.protocol.Scene>} value
 * @return {!proto.protocol.InitResponse} returns this
*/
proto.protocol.InitResponse.prototype.setScenesList = function(value) {
  return jspb.Message.setRepeatedWrapperField(this, 3, value);
};


/**
 * @param {!proto.protocol.Scene=} opt_value
 * @param {number=} opt_index
 * @return {!proto.protocol.Scene}
 */
proto.protocol.InitResponse.prototype.addScenes = function(opt_value, opt_index) {
  return jspb.Message.addToRepeatedWrapperField(this, 3, opt_value, proto.protocol.Scene, opt_index);
};


/**
 * Clears the list making it empty but non-null.
 * @return {!proto.protocol.InitResponse} returns this
 */
proto.protocol.InitResponse.prototype.clearScenesList = function() {
  return this.setScenesList([]);
};





if (jspb.Message.GENERATE_TO_OBJECT) {
/**
 * Creates an object representation of this proto.
 * Field names that are reserved in JavaScript and will be renamed to pb_name.
 * Optional fields that are not set will be set to undefined.
 * To access a reserved field use, foo.pb_<name>, eg, foo.pb_default.
 * For the list of reserved names please see:
 *     net/proto2/compiler/js/internal/generator.cc#kKeyword.
 * @param {boolean=} opt_includeInstance Deprecated. whether to include the
 *     JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @return {!Object}
 */
proto.protocol.Scene.prototype.toObject = function(opt_includeInstance) {
  return proto.protocol.Scene.toObject(opt_includeInstance, this);
};


/**
 * Static version of the {@see toObject} method.
 * @param {boolean|undefined} includeInstance Deprecated. Whether to include
 *     the JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @param {!proto.protocol.Scene} msg The msg instance to transform.
 * @return {!Object}
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.protocol.Scene.toObject = function(includeInstance, msg) {
  var f, obj = {
    name: jspb.Message.getFieldWithDefault(msg, 1, "")
  };

  if (includeInstance) {
    obj.$jspbMessageInstance = msg;
  }
  return obj;
};
}


/**
 * Deserializes binary data (in protobuf wire format).
 * @param {jspb.ByteSource} bytes The bytes to deserialize.
 * @return {!proto.protocol.Scene}
 */
proto.protocol.Scene.deserializeBinary = function(bytes) {
  var reader = new jspb.BinaryReader(bytes);
  var msg = new proto.protocol.Scene;
  return proto.protocol.Scene.deserializeBinaryFromReader(msg, reader);
};


/**
 * Deserializes binary data (in protobuf wire format) from the
 * given reader into the given message object.
 * @param {!proto.protocol.Scene} msg The message object to deserialize into.
 * @param {!jspb.BinaryReader} reader The BinaryReader to use.
 * @return {!proto.protocol.Scene}
 */
proto.protocol.Scene.deserializeBinaryFromReader = function(msg, reader) {
  while (reader.nextField()) {
    if (reader.isEndGroup()) {
      break;
    }
    var field = reader.getFieldNumber();
    switch (field) {
    case 1:
      var value = /** @type {string} */ (reader.readString());
      msg.setName(value);
      break;
    default:
      reader.skipField();
      break;
    }
  }
  return msg;
};


/**
 * Serializes the message to binary data (in protobuf wire format).
 * @return {!Uint8Array}
 */
proto.protocol.Scene.prototype.serializeBinary = function() {
  var writer = new jspb.BinaryWriter();
  proto.protocol.Scene.serializeBinaryToWriter(this, writer);
  return writer.getResultBuffer();
};


/**
 * Serializes the given message to binary data (in protobuf wire
 * format), writing to the given BinaryWriter.
 * @param {!proto.protocol.Scene} message
 * @param {!jspb.BinaryWriter} writer
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.protocol.Scene.serializeBinaryToWriter = function(message, writer) {
  var f = undefined;
  f = message.getName();
  if (f.length > 0) {
    writer.writeString(
      1,
      f
    );
  }
};


/**
 * optional string name = 1;
 * @return {string}
 */
proto.protocol.Scene.prototype.getName = function() {
  return /** @type {string} */ (jspb.Message.getFieldWithDefault(this, 1, ""));
};


/**
 * @param {string} value
 * @return {!proto.protocol.Scene} returns this
 */
proto.protocol.Scene.prototype.setName = function(value) {
  return jspb.Message.setProto3StringField(this, 1, value);
};


// goog.object.extend(exports, proto.protocol);
module.exports = proto.protocol;