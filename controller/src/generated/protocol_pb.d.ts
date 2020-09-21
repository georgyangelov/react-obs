// package: protocol
// file: protocol.proto

import * as jspb from "google-protobuf";

export class ClientMessage extends jspb.Message {
  hasInitRequest(): boolean;
  clearInitRequest(): void;
  getInitRequest(): InitRequest | undefined;
  setInitRequest(value?: InitRequest): void;

  hasApplyUpdates(): boolean;
  clearApplyUpdates(): void;
  getApplyUpdates(): ApplyUpdates | undefined;
  setApplyUpdates(value?: ApplyUpdates): void;

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
    applyUpdates?: ApplyUpdates.AsObject,
  }

  export enum MessageCase {
    MESSAGE_NOT_SET = 0,
    INIT_REQUEST = 1,
    APPLY_UPDATES = 2,
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

export class ApplyUpdates extends jspb.Message {
  clearUpdatesList(): void;
  getUpdatesList(): Array<Update>;
  setUpdatesList(value: Array<Update>): void;
  addUpdates(value?: Update, index?: number): Update;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): ApplyUpdates.AsObject;
  static toObject(includeInstance: boolean, msg: ApplyUpdates): ApplyUpdates.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: ApplyUpdates, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): ApplyUpdates;
  static deserializeBinaryFromReader(message: ApplyUpdates, reader: jspb.BinaryReader): ApplyUpdates;
}

export namespace ApplyUpdates {
  export type AsObject = {
    updatesList: Array<Update.AsObject>,
  }
}

export class Update extends jspb.Message {
  hasCreateElement(): boolean;
  clearCreateElement(): void;
  getCreateElement(): CreateElement | undefined;
  setCreateElement(value?: CreateElement): void;

  hasAppendChild(): boolean;
  clearAppendChild(): void;
  getAppendChild(): AppendChild | undefined;
  setAppendChild(value?: AppendChild): void;

  getChangeCase(): Update.ChangeCase;
  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): Update.AsObject;
  static toObject(includeInstance: boolean, msg: Update): Update.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: Update, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): Update;
  static deserializeBinaryFromReader(message: Update, reader: jspb.BinaryReader): Update;
}

export namespace Update {
  export type AsObject = {
    createElement?: CreateElement.AsObject,
    appendChild?: AppendChild.AsObject,
  }

  export enum ChangeCase {
    CHANGE_NOT_SET = 0,
    CREATE_ELEMENT = 1,
    APPEND_CHILD = 2,
  }
}

export class CreateElement extends jspb.Message {
  getType(): ElementTypeMap[keyof ElementTypeMap];
  setType(value: ElementTypeMap[keyof ElementTypeMap]): void;

  getName(): string;
  setName(value: string): void;

  getPropsMap(): jspb.Map<string, PropValue>;
  clearPropsMap(): void;
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
    propsMap: Array<[string, PropValue.AsObject]>,
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

export class PropValue extends jspb.Message {
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

  hasMapValue(): boolean;
  clearMapValue(): void;
  getMapValue(): MapProp | undefined;
  setMapValue(value?: MapProp): void;

  getValueCase(): PropValue.ValueCase;
  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): PropValue.AsObject;
  static toObject(includeInstance: boolean, msg: PropValue): PropValue.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: PropValue, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): PropValue;
  static deserializeBinaryFromReader(message: PropValue, reader: jspb.BinaryReader): PropValue;
}

export namespace PropValue {
  export type AsObject = {
    stringValue: string,
    intValue: number,
    floatValue: number,
    boolValue: boolean,
    mapValue?: MapProp.AsObject,
  }

  export enum ValueCase {
    VALUE_NOT_SET = 0,
    STRING_VALUE = 1,
    INT_VALUE = 2,
    FLOAT_VALUE = 3,
    BOOL_VALUE = 4,
    MAP_VALUE = 5,
  }
}

export class MapProp extends jspb.Message {
  getMapMap(): jspb.Map<string, PropValue>;
  clearMapMap(): void;
  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): MapProp.AsObject;
  static toObject(includeInstance: boolean, msg: MapProp): MapProp.AsObject;
  static extensions: {[key: number]: jspb.ExtensionFieldInfo<jspb.Message>};
  static extensionsBinary: {[key: number]: jspb.ExtensionFieldBinaryInfo<jspb.Message>};
  static serializeBinaryToWriter(message: MapProp, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): MapProp;
  static deserializeBinaryFromReader(message: MapProp, reader: jspb.BinaryReader): MapProp;
}

export namespace MapProp {
  export type AsObject = {
    mapMap: Array<[string, PropValue.AsObject]>,
  }
}

export interface ElementTypeMap {
  TEXT: 0;
  BOX: 1;
}

export const ElementType: ElementTypeMap;

