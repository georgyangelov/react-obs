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
export type Container = { uid: string, container: true };
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

export type FlexAlign =
  'auto' |
  'center' |
  'stretch' |
  'baseline' |
  'flex-start' |
  'flex-end' |
  'space-around' |
  'space-between';

interface ContainerStyle {
  flexDirection?: 'row' | 'column' | 'row-reverse' | 'column-reverse';
  direction?: 'inherit' | 'ltr' | 'rtl';
  justifyContent?: 'center' | 'flex-start' | 'flex-end' | 'space-around' | 'space-evenly' | 'space-between';
  alignContent?: FlexAlign;
  alignItems?: FlexAlign;
  flexWrap?: 'no-wrap' | 'wrap' | 'wrap-reverse';

  // padding?: string | number;
  paddingTop?: string | number;
  paddingLeft?: string | number;
  paddingRight?: string | number;
  paddingBottom?: string | number;
}

interface ItemStyle {
  alignSelf?: FlexAlign;
  position?: 'static' | 'relative' | 'absolute';

  // TODO: Support overflow
  // overflow?: 'visible' | 'scroll' | 'hidden';
  // display?: 'flex';

  flexGrow?: number;
  flexShrink?: number;
  flexBasis?: string | number;

  // `XXXpx`, `XXX%` or a number of pixels
  top?: string | number;
  left?: string | number;
  right?: string | number;
  bottom?: string | number;
  // margin?: string | number;
  marginTop?: string | number;
  marginLeft?: string | number;
  marginRight?: string | number;
  marginBottom?: string | number;
  width?: string | number;
  height?: string | number;
  minWidth?: string | number;
  maxWidth?: string | number;
  minHeight?: string | number;
  maxHeight?: string | number;

  aspectRatio?: number;
}

export type SceneStyle = ContainerStyle & ItemStyle;
export type SourceStyle = ItemStyle;
