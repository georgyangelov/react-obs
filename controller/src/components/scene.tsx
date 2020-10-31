import React, { ReactNode } from 'react';
import { SceneStyle } from '../types';

export function Scene({ name, style, children }: {
  name: string,
  style?: SceneStyle,
  children: ReactNode
}) {
  return (
    <obs_scene name={name} style={style}>{children}</obs_scene>
  );
}
