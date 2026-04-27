import { NitroModules } from 'react-native-nitro-modules';
import type { FastJson } from './FastJson.nitro';

const FastJsonHybridObject =
  NitroModules.createHybridObject<FastJson>('FastJson');

export function multiply(a: number, b: number): number {
  return FastJsonHybridObject.multiply(a, b);
}
