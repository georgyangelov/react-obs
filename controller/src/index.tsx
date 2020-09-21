import React, { ReactNode } from 'react';
import Reconciler, { OpaqueHandle } from 'react-reconciler';
import { createConnection } from 'net';
import { ServerAPI } from './server-api';
import { performance } from 'perf_hooks';
import {
  Type,
  Props,
  Container,
  Instance,
  TextInstance,
  HydratableInstance,
  PublicInstance,
  HostContext,
  UpdatePayload,
  ChildSet,
  TimeoutHandle,
  NoTimeout
} from './types';

console.log('Hello world');

const socket = createConnection({ port: 6666 }, () => {
  api.initialize();

  const root = reconciler.createContainer({ name: 'react-obs' }, false, false);
  reconciler.updateContainer(<Text>test</Text>, root, null, () => {
    console.log('done mounting');
  });
});
const api = new ServerAPI(socket);

function traceWrap<T extends { [k: string]: any }>(hostConfig: T): T {
  let traceWrappedHostConfig = {};
  Object.keys(hostConfig).map(key => {
    const func = hostConfig[key];
    // @ts-ignore
    traceWrappedHostConfig[key] = (...args) => {
      console.log(key, ...args);
      return func(...args);
    };
  });

  // @ts-ignore
  return traceWrappedHostConfig;
}

const ROOT_CONTEXT = {};



const reconciler = Reconciler<
  Type,
  Props,
  Container,
  Instance,
  TextInstance,
  HydratableInstance,
  PublicInstance,
  HostContext,
  UpdatePayload,
  ChildSet,
  TimeoutHandle,
  NoTimeout
>(traceWrap({
  now: performance.now,

  getPublicInstance(instance: Instance | TextInstance): PublicInstance {
    return instance;
  },

  getRootHostContext(rootContainerInstance: Container): HostContext {
    return ROOT_CONTEXT;
  },

  getChildHostContext(
    parentHostContext: HostContext,
    type: Type,
    rootContainerInstance: Container
  ): HostContext {
    return { childContext: true };
  },

  prepareForCommit(containerInfo: Container): void {

  },

  resetAfterCommit(containerInfo: Container): void {

  },

  createInstance(
    type: Type,
    props: Props,
    rootContainerInstance: Container,
    hostContext: HostContext,
    internalInstanceHandle: OpaqueHandle,
  ): Instance {
    Object.keys(props).forEach(key => {
      const value = props[key];
      if (key === 'children') {
        if (type !== 'obs_text') {
          if (typeof value === 'string' || typeof value === 'number') {
            throw new Error('Text strings must be rendered within a <Text> component.');
          }

          if (value instanceof Array) {
            value.forEach(item => {
              if (typeof item === 'string') {
                throw new Error('Text strings must be rendered within a <Text> component.');
              }
            });
          }
        }

        if (type === 'obs_text') {
          if (typeof value !== 'string' && typeof value !== 'number') {
            throw new Error('A <Text> component may only render strings or numbers, not other elements');
          }
        }
      }
    });

    return api.createElement(type, props);
  },

  appendInitialChild(parent: Instance, child: Instance | TextInstance): void {
    api.appendChild(parent, child);
  },

  finalizeInitialChildren(
    parentInstance: Instance,
    type: Type,
    props: Props,
    rootContainerInstance: Container,
    hostContext: HostContext,
  ): boolean {
    return false;
  },

  prepareUpdate(
    instance: Instance,
    type: Type,
    oldProps: Props,
    newProps: Props,
    rootContainerInstance: Container,
    hostContext: HostContext,
  ): null | UpdatePayload {
    return { updatePayload: true };
  },

  shouldSetTextContent(type: Type, props: Props): boolean {
    return type === 'obs_text';
  },

  shouldDeprioritizeSubtree(type: Type, props: Props): boolean {
    return false;
  },

  createTextInstance(
    text: string,
    rootContainerInstance: Container,
    hostContext: HostContext,
    internalInstanceHandle: OpaqueHandle,
  ): TextInstance {
    return api.createElement('obs_text', { text });
  },

  // TODO: Is this requestAnimationFrame?
  scheduleDeferredCallback(
    callback: () => any,
    options?: { timeout: number },
  ): any {
    return setTimeout(callback, options?.timeout ?? 0);
  },

  cancelDeferredCallback(callbackID: any): void {
    clearTimeout(callbackID);
  },

  setTimeout(handler: (...args: any[]) => void, timeout: number): TimeoutHandle | NoTimeout {
    return setTimeout(handler, timeout);
  },

  clearTimeout(handle: TimeoutHandle | NoTimeout): void {
    if (handle) {
      clearTimeout(handle);
    }
  },

  noTimeout: undefined,

  // Temporary workaround for scenario where multiple renderers concurrently
  // render using the same context objects. E.g. React DOM and React ART on the
  // same page. DOM is the primary renderer; ART is the secondary renderer.
  isPrimaryRenderer: true,

  supportsMutation: true,
  supportsPersistence: false,
  supportsHydration: false,

  // -------------------
  //      Mutation
  //     (optional)
  // -------------------
  appendChild(parent: Instance, child: Instance | TextInstance): void {
    api.appendChild(parent, child);
  },

  appendChildToContainer(container: Container, child: Instance | TextInstance): void {
    // TODO: What is the difference between this and appendChild?
    api.appendChild(container, child);
  },

  commitTextUpdate(textInstance: TextInstance, oldText: string, newText: string): void {

  },

  commitMount(
    instance: Instance,
    type: Type,
    newProps: Props,
    internalInstanceHandle: OpaqueHandle,
  ): void {

  },

  commitUpdate(
    instance: Instance,
    updatePayload: UpdatePayload,
    type: Type,
    oldProps: Props,
    newProps: Props,
    internalInstanceHandle: OpaqueHandle,
  ): void {

  },

  insertBefore(
    parentInstance: Instance,
    child: Instance | TextInstance,
    beforeChild: Instance | TextInstance
  ): void {

  },

  insertInContainerBefore(
    container: Container,
    child: Instance | TextInstance,
    beforeChild: Instance | TextInstance,
  ): void {

  },

  removeChild(
    parentInstance: Instance,
    child: Instance | TextInstance
  ): void {

  },

  removeChildFromContainer(
    container: Container,
    child: Instance | TextInstance
  ): void {

  },

  resetTextContent(instance: Instance): void {

  },

  // -------------------
  //     Persistence
  //     (optional)
  // -------------------
  // cloneInstance?(
  //     instance: Instance,
  //     updatePayload: null | UpdatePayload,
  //     type: Type,
  //     oldProps: Props,
  //     newProps: Props,
  //     internalInstanceHandle: OpaqueHandle,
  //     keepChildren: boolean,
  //     recyclableInstance: Instance,
  // ): Instance;
  //
  // createContainerChildSet?(container: Container): ChildSet;
  //
  // appendChildToContainerChildSet?(childSet: ChildSet, child: Instance | TextInstance): void;
  // finalizeContainerChildren?(container: Container, newChildren: ChildSet): void;
  //
  // replaceContainerChildren?(container: Container, newChildren: ChildSet): void;

  // -------------------
  //     Hydration
  //     (optional)
  // -------------------
  // canHydrateInstance?(instance: HydratableInstance, type: Type, props: Props): null | Instance;
  // canHydrateTextInstance?(instance: HydratableInstance, text: string): null | TextInstance;
  // getNextHydratableSibling?(instance: Instance | TextInstance | HydratableInstance): null | HydratableInstance;
  // getFirstHydratableChild?(parentInstance: Instance | Container): null | HydratableInstance;
  // hydrateInstance?(
  //     instance: Instance,
  //     type: Type,
  //     props: Props,
  //     rootContainerInstance: Container,
  //     hostContext: HostContext,
  //     internalInstanceHandle: OpaqueHandle,
  // ): null | UpdatePayload;
  // hydrateTextInstance?(
  //     textInstance: TextInstance,
  //     text: string,
  //     internalInstanceHandle: OpaqueHandle,
  // ): boolean;
  // didNotMatchHydratedContainerTextInstance?(
  //     parentContainer: Container,
  //     textInstance: TextInstance,
  //     text: string,
  // ): void;
  // didNotMatchHydratedTextInstance?(
  //     parentType: Type,
  //     parentProps: Props,
  //     parentInstance: Instance,
  //     textInstance: TextInstance,
  //     text: string,
  // ): void;
  // didNotHydrateContainerInstance?(parentContainer: Container, instance: Instance | TextInstance): void;
  // didNotHydrateInstance?(
  //     parentType: Type,
  //     parentProps: Props,
  //     parentInstance: Instance,
  //     instance: Instance | TextInstance,
  // ): void;
  // didNotFindHydratableContainerInstance?(
  //     parentContainer: Container,
  //     type: Type,
  //     props: Props,
  // ): void;
  // didNotFindHydratableContainerTextInstance?(
  //     parentContainer: Container,
  //     text: string,
  // ): void;
  // didNotFindHydratableInstance?(
  //     parentType: Type,
  //     parentProps: Props,
  //     parentInstance: Instance,
  //     type: Type,
  //     props: Props,
  // ): void;
  // didNotFindHydratableTextInstance?(
  //     parentType: Type,
  //     parentProps: Props,
  //     parentInstance: Instance,
  //     text: string,
  // ): void;
}));

// function Text() {
//   console.log('rendered Text');
//
//   return null;
// }

function Text({ children }: { children: ReactNode }) {
  return <obs_text>{children}</obs_text>;
}