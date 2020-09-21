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
  }

  export enum MessageCase {
    MESSAGE_NOT_SET = 0,
    INIT_REQUEST = 1,
    APPLY_UPDATE = 2,
  }
}

export class ServerMessage extends jspb.Message {
  hasInitResponse(): boolean;
  clearInitResponse(): void;
  getInitResponse(): InitResponse | undefined;
  setInitResponse(value?: InitResponse): void;

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
    initResponse?: InitResponse.AsObject,
  }

  export enum MessageCase {
    MESSAGE_NOT_SET = 0,
    INIT_RESPONSE = 1,
  }
}

export class InitRequest extends jspb.Message {
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
    clientId: string,
  }
}

export class InitResponse extends jspb.Message {
  getCanvasWidth(): number;
  setCanvasWidth(value: number): void;

  getCanvasHeight(): number;
  setCanvasHeight(value: number): void;

  clearScenesList(): void;
  getScenesList(): Array<Scene>;
  setScenesList(value: Array<Scene>): void;
  addScenes(value?: Scene, index?: number): Scene;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): InitResponse.AsObject;
  static toObject(includeInstance: boolean, msg: InitResponse): InitResponse.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: InitResponse, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): InitResponse;
  static deserializeBinaryFromReader(message: InitResponse, reader: jspb.BinaryReader): InitResponse;
}

export namespace InitResponse {
  export type AsObject = {
    canvasWidth: number,
    canvasHeight: number,
    scenesList: Array<Scene.AsObject>,
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
  hasCreateElement(): boolean;
  clearCreateElement(): void;
  getCreateElement(): CreateElement | undefined;
  setCreateElement(value?: CreateElement): void;

  hasUpdateElement(): boolean;
  clearUpdateElement(): void;
  getUpdateElement(): UpdateElement | undefined;
  setUpdateElement(value?: UpdateElement): void;

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
    createElement?: CreateElement.AsObject,
    updateElement?: UpdateElement.AsObject,
    appendChild?: AppendChild.AsObject,
    removeChild?: RemoveChild.AsObject,
  }

  export enum ChangeCase {
    CHANGE_NOT_SET = 0,
    CREATE_ELEMENT = 1,
    UPDATE_ELEMENT = 3,
    APPEND_CHILD = 2,
    REMOVE_CHILD = 4,
  }
}

export class CreateElement extends jspb.Message {
  getType(): ElementTypeMap[keyof ElementTypeMap];
  setType(value: ElementTypeMap[keyof ElementTypeMap]): void;

  getName(): string;
  setName(value: string): void;

  clearPropsList(): void;
  getPropsList(): Array<Prop>;
  setPropsList(value: Array<Prop>): void;
  addProps(value?: Prop, index?: number): Prop;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): CreateElement.AsObject;
  static toObject(includeInstance: boolean, msg: CreateElement): CreateElement.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: CreateElement, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): CreateElement;
  static deserializeBinaryFromReader(message: CreateElement, reader: jspb.BinaryReader): CreateElement;
}

export namespace CreateElement {
  export type AsObject = {
    type: ElementTypeMap[keyof ElementTypeMap],
    name: string,
    propsList: Array<Prop.AsObject>,
  }
}

export class UpdateElement extends jspb.Message {
  getType(): ElementTypeMap[keyof ElementTypeMap];
  setType(value: ElementTypeMap[keyof ElementTypeMap]): void;

  getName(): string;
  setName(value: string): void;

  clearChangedPropsList(): void;
  getChangedPropsList(): Array<Prop>;
  setChangedPropsList(value: Array<Prop>): void;
  addChangedProps(value?: Prop, index?: number): Prop;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): UpdateElement.AsObject;
  static toObject(includeInstance: boolean, msg: UpdateElement): UpdateElement.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: UpdateElement, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): UpdateElement;
  static deserializeBinaryFromReader(message: UpdateElement, reader: jspb.BinaryReader): UpdateElement;
}

export namespace UpdateElement {
  export type AsObject = {
    type: ElementTypeMap[keyof ElementTypeMap],
    name: string,
    changedPropsList: Array<Prop.AsObject>,
  }
}

export class AppendChild extends jspb.Message {
  getParentName(): string;
  setParentName(value: string): void;

  getChildName(): string;
  setChildName(value: string): void;

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
    parentName: string,
    childName: string,
  }
}

export class RemoveChild extends jspb.Message {
  getParentName(): string;
  setParentName(value: string): void;

  getChildName(): string;
  setChildName(value: string): void;

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
    parentName: string,
    childName: string,
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
  getObjectValue(): Object | undefined;
  setObjectValue(value?: Object): void;

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
    objectValue?: Object.AsObject,
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

export class Object extends jspb.Message {
  clearPropsList(): void;
  getPropsList(): Array<Prop>;
  setPropsList(value: Array<Prop>): void;
  addProps(value?: Prop, index?: number): Prop;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): Object.AsObject;
  static toObject(includeInstance: boolean, msg: Object): Object.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: Object, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): Object;
  static deserializeBinaryFromReader(message: Object, reader: jspb.BinaryReader): Object;
}

export namespace Object {
  export type AsObject = {
    propsList: Array<Prop.AsObject>,
  }
}

export interface ElementTypeMap {
  TEXT: 0;
  BOX: 1;
}

export const ElementType: ElementTypeMap;

