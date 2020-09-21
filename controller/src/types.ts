declare global {
  namespace JSX {
    interface IntrinsicElements {
      obs_box: any;
      obs_text: any;
    }
  }
}

export type Type = 'obs_text' | 'obs_box';
export type Props = { [key: string]: string | number | boolean | Props };
export type Container = Instance;
export type Instance = { name: string };
export type TextInstance = Instance;
export type HydratableInstance = any;
export type PublicInstance = Instance;
export type HostContext = {};
export type UpdatePayload = { propChanges: PropChange[] };
export type ChildSet = any;
export type TimeoutHandle = NodeJS.Timeout;
export type NoTimeout = undefined;

export type PropChange = { key: string, value: Props[string] | undefined };
