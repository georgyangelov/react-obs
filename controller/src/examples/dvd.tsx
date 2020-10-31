import React from 'react';
import path from 'path';
import { ReactOBS } from '..';
import { Image } from '../components/image';
import { Scene } from '../components/scene';
import { useTime } from '../hooks/use-time';


const IMAGE_PATH = path.join(__dirname, 'assets/dvd-logo.jpg');

const SPEED_X = 0.2;
const SPEED_Y = 0.1;
const START_X = 20;
const START_Y = 3;

const IMAGE_WIDTH = 289;
const IMAGE_HEIGHT = 184;

const SCENE_WIDTH = 1920;
const SCENE_HEIGHT = 1080;

const styles = {
  root: {
    position: 'relative',
    width: '100%',
    height: '100%',
  },
} as const;

function wrapAlternate(value: number, min: number, max: number) {
  const fraction = (value - min) / (max - min) % 1;
  const alternateFraction = 1 - Math.abs(fraction * 2 - 1);

  return alternateFraction * (max - min) + min;
}

function App() {
  const time = useTime();

  const x = wrapAlternate(START_X + time * SPEED_X, 0, SCENE_WIDTH - IMAGE_WIDTH);
  const y = wrapAlternate(START_Y + time * SPEED_Y, 0, SCENE_HEIGHT - IMAGE_HEIGHT);

  const logoStyle = {
    position: 'absolute',
    top: y,
    left: x,

    width: IMAGE_WIDTH,
    height: IMAGE_HEIGHT
  } as const;

  return (
    <Scene name="React Scene" style={styles.root}>
      <Image name="DVD Logo" style={logoStyle} path={IMAGE_PATH} />
      {/* <Text
        name="React Text"
        style={{ ...styles.text, ...position }}
        fontSize={42}
        fontFace="Fira Code"
        fontStyle="Bold">Elapsed Time: {Math.round(time)}ms</Text> */}
    </Scene>
  );
}

(async function() {
  const obs = await ReactOBS.connect('localhost', 6666);
  const scene = await obs.findScene('react-obs');

  obs.render(scene, <App />);
})();
