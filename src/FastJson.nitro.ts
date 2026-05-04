import type { AnyMap, HybridObject } from 'react-native-nitro-modules';

interface JsonView extends HybridObject<{
  ios: 'c++';
  android: 'c++';
}> {
  toJson(): AnyMap;
  toBuffer(): ArrayBuffer;
  getValue(key: string): JsonView | null;
  keys(): string[];
  has(key: string): boolean;
  at(index: number): JsonView | null;
  type: AnyMap;
  length: number;
  asString(): string;
  asNumber(): number;
  asBoolean(): boolean;
}

type JsonSourceUri = {
  uri: string;
};
type JsonSource = string | JsonSourceUri;

export interface FastJson extends HybridObject<{
  ios: 'c++';
  android: 'c++';
}> {
  parse(source: JsonSource): Promise<JsonView | null>;
}
