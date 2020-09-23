export function traceWrap<T extends { [k: string]: any }>(hostConfig: T): T {
  let traceWrappedHostConfig = {};
  Object.keys(hostConfig).map(key => {
    const func = hostConfig[key];
    // @ts-ignore
    traceWrappedHostConfig[key] = (...args) => {
      console.log(key, ...args);
      return func(...args);
    };
  });

  // @ts-ignore
  return traceWrappedHostConfig;
}
