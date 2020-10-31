import { ReactNode } from 'react';
import { createConnection, Socket } from 'net';
import { ServerAPI } from './server-api';
import { reconciler } from './react-reconciler';
import { Container } from './types';

export class ReactOBS {
  static connect(host: string, port: number): Promise<ReactOBS> {
    return new Promise((resolve, reject) => {
      const socket: Socket = createConnection({ host, port }, async () => {
        try {
          await api.initialize();
        } catch (error) {
          reject(error);
          return;
        }

        resolve(new ReactOBS(api));
      });
      const api = new ServerAPI(socket);

      socket.once('error', reject);
    });
  }

  private reconciler = reconciler(this.api);

  constructor(private api: ServerAPI) {}

  render(container: Container, element: ReactNode): Promise<void> {
    return new Promise((resolve, _reject) => {
      const root = this.reconciler.createContainer(container, false, false);

      this.reconciler.updateContainer(element, root, null, resolve);
    });
  }

  findScene(name: string): Promise<Container> {
    return this.api.findContainer(name);
  }
}
