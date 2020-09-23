import React, { useMemo } from 'react';

export function Text({ name, children, fontSize, fontFace, fontStyle }: {
  name: string,
  children: string | number | (string | number)[],
  fontSize?: number,
  fontFace?: string,
  fontStyle?: string
}) {
  const text = children instanceof Array ? children.join('') : children;

  const font = useMemo(() => ({
    size: fontSize,
    face: fontFace,
    style: fontStyle
  }), [fontSize, fontFace, fontStyle]);

  return (
    <obs_source
      id="text_ft2_source"
      name={name}
      font={font}
      text={text} />
  );
}
