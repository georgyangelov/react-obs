import React from 'react';
import { SourceStyle } from '../types';

export function Image({ name, path, style, unloadWhenNotShowing = false }: {
  name: string,
  path: string,
  unloadWhenNotShowing?: boolean,
  style?: SourceStyle
}) {
  return (
    <obs_source
      id="image_source"
      name={name}
      style={style}
      file={path}
      unload={unloadWhenNotShowing} />
  );
}
