import React, { ReactNode } from 'react';

export function Scene({ name, children }: {
  name: string,
  children: ReactNode
}) {
  return (
    <obs_scene
      name={name}
      flexDirection="row"
    >{children}</obs_scene>
  );
}
