import type { HybridObject } from 'react-native-nitro-modules';

interface JsonView extends HybridObject<{
  ios: 'c++';
  android: 'c++';
}> {
  rawJson(): string;
  toBuffer(): ArrayBuffer;
  getValue(key: string): JsonView | null;
  keys(): string[];
  has(key: string): boolean;
  at(index: number): JsonView | null;
  atPath(path: string /* key1[2].key2/key3[0] */): JsonView | null;
  atPathWithWildcard(path: string /* $.key1[*].key2[1] */): string[] | null;
  type: string;
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
