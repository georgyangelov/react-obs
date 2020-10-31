import React, { useEffect, useState } from 'react';
import { ReactOBS } from '..';
import { Scene } from '../components/scene';
import { Text } from '../components/text';

const styles = {
  root: {
    flexDirection: 'row'
  },

  centerText: {
    flexGrow: 1
  }
} as const;

function App() {
  const [counter, setCounter] = useState(0);

  useEffect(() => {
    const interval = setInterval(() => {
      setCounter(count => count + 1);
    }, 1000);

    return () => clearInterval(interval);
  }, []);

  // if (counter % 2 === 0) {
  //   return null;
  // }

  return (
    <Scene name="React Scene" style={styles.root}>
      <Text
        name="React Text"
        fontSize={42}
        fontFace="Fira Code"
        fontStyle="Bold">Counter: {counter}</Text>

      <Text
        style={styles.centerText}
        name="React Text"
        fontSize={42}
        fontFace="Fira Code"
        fontStyle="Bold">Counter: {counter}{counter}</Text>

      <Text
        name="React Text"
        fontSize={42}
        fontFace="Fira Code"
        fontStyle="Bold">Counter: {counter}{counter}{counter}</Text>
    </Scene>
  );
}

(async function() {
  const obs = await ReactOBS.connect('localhost', 6666);
  const scene = await obs.findScene('react-obs');

  obs.render(scene, <App />);
})();
