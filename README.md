# React-OBS

## What is this?

A React renderer capable of controlling OBS. Idea is to layout OBS scenes using React, making it easy to build interactive plugins.

It's currently a work in progress.

## What will it be?

Imagine laying out scenes with React and flexbox:

```jsx
<Scene name="Camera Only" alignItems="center" justiftContent="center">
  <Webcam />
</Scene>

<Scene name="Picture-in-Picture">
  <ScreenCapture
    position="absolute"
    top="20"
    left="20"
    width="80%"
    height="80%" />

  <Webcam
    position="absolute"
    bottom="20"
    right="20"
    width="20%"
    height="20%" />
</Scene>

<Scene name="Side-by-Side" alignItems="center" justifyContent="space-between" padding="20">
  <ScreenCapture marginRight="20" />
  <Webcam />
</Scene>
```

Also imagine smart components that can react to external events:

```js
function Countdown({ startTime, duration }) {
  const time = useTime();
  const timeLeft = duration - (time - startTime);

  if (timeLeft < 0) {
    return (
      <Text fontColor="#ff0000">Done!</Text>
    );
  }

  return (
    <Text fontColor="#00ff00">Counting down: {timeLeft}s</Text>
  );
}

ReactOBS.connect('localhost', 6666);

ReactOBS.render(
  ReactOBS.findScene('countdown'),
  <Countdown startTime={new Date()} duration={60 * 1000} />
);
```

## How does it work?

The architecture is as follows:

- The `controller` is a nodejs project which implements a react renderer (similar to ReactDOM and ReactNative).
- Whenever React wants to make changes to the "DOM", the `controller` sends a message to a custom OBS plugin (`obs-plugin` dir). They communicate via sockets (currently TCP) using protobuf structs.
- The OBS plugin uses the OBS C API to manage scenes and elements. It is also responsible for computing layout using Facebook's Yoga library - the same one that React Native uses.
