import { performance } from "perf_hooks";
import { useEffect, useState } from "react";

// TODO: This is inadequate
export function useTime() {
  const [time, setTime] = useState<number>(() => performance.now());

  useEffect(() => {
    const interval = setInterval(() => {
      setTime(performance.now());
    }, 8);

    return () => clearInterval(interval);
  }, []);

  return time;
}
