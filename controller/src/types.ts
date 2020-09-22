declare global {
  namespace JSX {
    interface IntrinsicElements {
      obs_source: any;
      obs_scene: any;
    }
  }
}

export type Type = 'obs_source' | 'obs_scene';
export type Props = { [key: string]: string | number | boolean | Props };
export type Container = { uid: string, unmanaged: true };
export type Instance = { uid: string, unmanaged?: boolean };
export type TextInstance = Instance;
export type HydratableInstance = any;
export type PublicInstance = Instance;
export type HostContext = {};
export type UpdatePayload = { propChanges: PropChanges };
export type ChildSet = any;
export type TimeoutHandle = NodeJS.Timeout;
export type NoTimeout = undefined;

export type PropChanges = { [key: string]: string | number | boolean | Props | undefined };
