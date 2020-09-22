declare global {
  namespace JSX {
    interface IntrinsicElements {
      obs_source: any;
    }
  }
}

export type Type = 'obs_source';
export type Props = { [key: string]: string | number | boolean | Props };
export type Container = Instance;
export type Instance = { name: string };
export type TextInstance = Instance;
export type HydratableInstance = any;
export type PublicInstance = Instance;
export type HostContext = {};
export type UpdatePayload = { propChanges: PropChanges };
export type ChildSet = any;
export type TimeoutHandle = NodeJS.Timeout;
export type NoTimeout = undefined;

export type PropChanges = { [key: string]: string | number | boolean | Props | undefined };
