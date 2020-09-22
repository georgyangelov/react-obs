// package: protocol
// file: protocol.proto

import * as jspb from "google-protobuf";

export class ClientMessage extends jspb.Message {
  hasInitRequest(): boolean;
  clearInitRequest(): void;
  getInitRequest(): InitRequest | undefined;
  setInitRequest(value?: InitRequest): void;

  hasApplyUpdate(): boolean;
  clearApplyUpdate(): void;
  getApplyUpdate(): ApplyUpdate | undefined;
  setApplyUpdate(value?: ApplyUpdate): void;

  hasFindSource(): boolean;
  clearFindSource(): void;
  getFindSource(): FindSourceRequest | undefined;
  setFindSource(value?: FindSourceRequest): void;

  getMessageCase(): ClientMessage.MessageCase;
  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): ClientMessage.AsObject;
  static toObject(includeInstance: boolean, msg: ClientMessage): ClientMessage.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: ClientMessage, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): ClientMessage;
  static deserializeBinaryFromReader(message: ClientMessage, reader: jspb.BinaryReader): ClientMessage;
}

export namespace ClientMessage {
  export type AsObject = {
    initRequest?: InitRequest.AsObject,
    applyUpdate?: ApplyUpdate.AsObject,
    findSource?: FindSourceRequest.AsObject,
  }

  export enum MessageCase {
    MESSAGE_NOT_SET = 0,
    INIT_REQUEST = 1,
    APPLY_UPDATE = 2,
    FIND_SOURCE = 3,
  }
}

export class ServerMessage extends jspb.Message {
  hasResponse(): boolean;
  clearResponse(): void;
  getResponse(): Response | undefined;
  setResponse(value?: Response): void;

  getMessageCase(): ServerMessage.MessageCase;
  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): ServerMessage.AsObject;
  static toObject(includeInstance: boolean, msg: ServerMessage): ServerMessage.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: ServerMessage, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): ServerMessage;
  static deserializeBinaryFromReader(message: ServerMessage, reader: jspb.BinaryReader): ServerMessage;
}

export namespace ServerMessage {
  export type AsObject = {
    response?: Response.AsObject,
  }

  export enum MessageCase {
    MESSAGE_NOT_SET = 0,
    RESPONSE = 1,
  }
}

export class InitRequest extends jspb.Message {
  getRequestId(): string;
  setRequestId(value: string): void;

  getClientId(): string;
  setClientId(value: string): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): InitRequest.AsObject;
  static toObject(includeInstance: boolean, msg: InitRequest): InitRequest.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: InitRequest, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): InitRequest;
  static deserializeBinaryFromReader(message: InitRequest, reader: jspb.BinaryReader): InitRequest;
}

export namespace InitRequest {
  export type AsObject = {
    requestId: string,
    clientId: string,
  }
}

export class Scene extends jspb.Message {
  getName(): string;
  setName(value: string): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): Scene.AsObject;
  static toObject(includeInstance: boolean, msg: Scene): Scene.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: Scene, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): Scene;
  static deserializeBinaryFromReader(message: Scene, reader: jspb.BinaryReader): Scene;
}

export namespace Scene {
  export type AsObject = {
    name: string,
  }
}

export class ApplyUpdate extends jspb.Message {
  hasCreateSource(): boolean;
  clearCreateSource(): void;
  getCreateSource(): CreateSource | undefined;
  setCreateSource(value?: CreateSource): void;

  hasUpdateSource(): boolean;
  clearUpdateSource(): void;
  getUpdateSource(): UpdateSource | undefined;
  setUpdateSource(value?: UpdateSource): void;

  hasAppendChild(): boolean;
  clearAppendChild(): void;
  getAppendChild(): AppendChild | undefined;
  setAppendChild(value?: AppendChild): void;

  hasRemoveChild(): boolean;
  clearRemoveChild(): void;
  getRemoveChild(): RemoveChild | undefined;
  setRemoveChild(value?: RemoveChild): void;

  getChangeCase(): ApplyUpdate.ChangeCase;
  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): ApplyUpdate.AsObject;
  static toObject(includeInstance: boolean, msg: ApplyUpdate): ApplyUpdate.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: ApplyUpdate, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): ApplyUpdate;
  static deserializeBinaryFromReader(message: ApplyUpdate, reader: jspb.BinaryReader): ApplyUpdate;
}

export namespace ApplyUpdate {
  export type AsObject = {
    createSource?: CreateSource.AsObject,
    updateSource?: UpdateSource.AsObject,
    appendChild?: AppendChild.AsObject,
    removeChild?: RemoveChild.AsObject,
  }

  export enum ChangeCase {
    CHANGE_NOT_SET = 0,
    CREATE_SOURCE = 1,
    UPDATE_SOURCE = 3,
    APPEND_CHILD = 2,
    REMOVE_CHILD = 4,
  }
}

export class FindSourceRequest extends jspb.Message {
  getRequestId(): string;
  setRequestId(value: string): void;

  getUid(): string;
  setUid(value: string): void;

  getName(): string;
  setName(value: string): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): FindSourceRequest.AsObject;
  static toObject(includeInstance: boolean, msg: FindSourceRequest): FindSourceRequest.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: FindSourceRequest, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): FindSourceRequest;
  static deserializeBinaryFromReader(message: FindSourceRequest, reader: jspb.BinaryReader): FindSourceRequest;
}

export namespace FindSourceRequest {
  export type AsObject = {
    requestId: string,
    uid: string,
    name: string,
  }
}

export class Response extends jspb.Message {
  getRequestId(): string;
  setRequestId(value: string): void;

  getSuccess(): boolean;
  setSuccess(value: boolean): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): Response.AsObject;
  static toObject(includeInstance: boolean, msg: Response): Response.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: Response, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): Response;
  static deserializeBinaryFromReader(message: Response, reader: jspb.BinaryReader): Response;
}

export namespace Response {
  export type AsObject = {
    requestId: string,
    success: boolean,
  }
}

export class CreateSource extends jspb.Message {
  getUid(): string;
  setUid(value: string): void;

  getId(): string;
  setId(value: string): void;

  getName(): string;
  setName(value: string): void;

  hasSettings(): boolean;
  clearSettings(): void;
  getSettings(): ObjectValue | undefined;
  setSettings(value?: ObjectValue): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): CreateSource.AsObject;
  static toObject(includeInstance: boolean, msg: CreateSource): CreateSource.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: CreateSource, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): CreateSource;
  static deserializeBinaryFromReader(message: CreateSource, reader: jspb.BinaryReader): CreateSource;
}

export namespace CreateSource {
  export type AsObject = {
    uid: string,
    id: string,
    name: string,
    settings?: ObjectValue.AsObject,
  }
}

export class UpdateSource extends jspb.Message {
  getUid(): string;
  setUid(value: string): void;

  hasChangedProps(): boolean;
  clearChangedProps(): void;
  getChangedProps(): ObjectValue | undefined;
  setChangedProps(value?: ObjectValue): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): UpdateSource.AsObject;
  static toObject(includeInstance: boolean, msg: UpdateSource): UpdateSource.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: UpdateSource, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): UpdateSource;
  static deserializeBinaryFromReader(message: UpdateSource, reader: jspb.BinaryReader): UpdateSource;
}

export namespace UpdateSource {
  export type AsObject = {
    uid: string,
    changedProps?: ObjectValue.AsObject,
  }
}

export class AppendChild extends jspb.Message {
  getParentUid(): string;
  setParentUid(value: string): void;

  getChildUid(): string;
  setChildUid(value: string): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): AppendChild.AsObject;
  static toObject(includeInstance: boolean, msg: AppendChild): AppendChild.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: AppendChild, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): AppendChild;
  static deserializeBinaryFromReader(message: AppendChild, reader: jspb.BinaryReader): AppendChild;
}

export namespace AppendChild {
  export type AsObject = {
    parentUid: string,
    childUid: string,
  }
}

export class RemoveChild extends jspb.Message {
  getParentUid(): string;
  setParentUid(value: string): void;

  getChildUid(): string;
  setChildUid(value: string): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): RemoveChild.AsObject;
  static toObject(includeInstance: boolean, msg: RemoveChild): RemoveChild.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: RemoveChild, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): RemoveChild;
  static deserializeBinaryFromReader(message: RemoveChild, reader: jspb.BinaryReader): RemoveChild;
}

export namespace RemoveChild {
  export type AsObject = {
    parentUid: string,
    childUid: string,
  }
}

export class Prop extends jspb.Message {
  getKey(): string;
  setKey(value: string): void;

  hasStringValue(): boolean;
  clearStringValue(): void;
  getStringValue(): string;
  setStringValue(value: string): void;

  hasIntValue(): boolean;
  clearIntValue(): void;
  getIntValue(): number;
  setIntValue(value: number): void;

  hasFloatValue(): boolean;
  clearFloatValue(): void;
  getFloatValue(): number;
  setFloatValue(value: number): void;

  hasBoolValue(): boolean;
  clearBoolValue(): void;
  getBoolValue(): boolean;
  setBoolValue(value: boolean): void;

  hasObjectValue(): boolean;
  clearObjectValue(): void;
  getObjectValue(): ObjectValue | undefined;
  setObjectValue(value?: ObjectValue): void;

  hasUndefined(): boolean;
  clearUndefined(): void;
  getUndefined(): boolean;
  setUndefined(value: boolean): void;

  getValueCase(): Prop.ValueCase;
  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): Prop.AsObject;
  static toObject(includeInstance: boolean, msg: Prop): Prop.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: Prop, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): Prop;
  static deserializeBinaryFromReader(message: Prop, reader: jspb.BinaryReader): Prop;
}

export namespace Prop {
  export type AsObject = {
    key: string,
    stringValue: string,
    intValue: number,
    floatValue: number,
    boolValue: boolean,
    objectValue?: ObjectValue.AsObject,
    undefined: boolean,
  }

  export enum ValueCase {
    VALUE_NOT_SET = 0,
    STRING_VALUE = 2,
    INT_VALUE = 3,
    FLOAT_VALUE = 4,
    BOOL_VALUE = 5,
    OBJECT_VALUE = 6,
    UNDEFINED = 7,
  }
}

export class ObjectValue extends jspb.Message {
  clearPropsList(): void;
  getPropsList(): Array<Prop>;
  setPropsList(value: Array<Prop>): void;
  addProps(value?: Prop, index?: number): Prop;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): ObjectValue.AsObject;
  static toObject(includeInstance: boolean, msg: ObjectValue): ObjectValue.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: ObjectValue, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): ObjectValue;
  static deserializeBinaryFromReader(message: ObjectValue, reader: jspb.BinaryReader): ObjectValue;
}

export namespace ObjectValue {
  export type AsObject = {
    propsList: Array<Prop.AsObject>,
  }
}

