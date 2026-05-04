import { NitroModules } from 'react-native-nitro-modules';

import type { FastJson } from './FastJson.nitro';

export const fastJson = NitroModules.createHybridObject<FastJson>('FastJson');
