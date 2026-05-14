import { NitroModules } from 'react-native-nitro-modules';

import type { FastJson, JsonView } from './FastJson.nitro';

export const fastJson = NitroModules.createHybridObject<FastJson>('FastJson');
export type { JsonView };
