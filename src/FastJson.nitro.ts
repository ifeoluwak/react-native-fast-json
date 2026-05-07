import type { HybridObject } from 'react-native-nitro-modules';

interface JsonView extends HybridObject<{
  ios: 'c++';
  android: 'c++';
}> {
  /**
   * Returns the raw JSON string of the JSON view.
   * @returns The raw JSON string of the JSON view.
   */
  rawJson(): string;
  toBuffer(): ArrayBuffer;
  /**
   * Returns the value of the key in the JSON view.
   * @param key The key to get the value of. Must be a single key, not a path.
   * @returns The value of the key in the JSON view.
   */
  getValue(key: string): JsonView | null;
  /**
   * Returns all the keys or fields of an object or array.
   * Only for objects or arrays.
   * @returns The keys of the JSON view.
   */
  keys(): string[];
  has(key: string): boolean;
  /**
   * Only for arrays. Returns the value of the index of an array.
   * @param index The index to get the value from an array.
   * @returns The value of the index in the array.
   */
  at(index: number): JsonView | null;
  /**
   * Returns all the values of the path specified.
   * @param path simple path e.g key1.key2.key3. Indexing is not supported.
   * @returns Returns a JsonView.
   */
  atPath(path: string /* key1[2].key2/key3[0] */): JsonView | null;
  /**
   * For dynamic retrieval of nested values with wildcards.
   * Supports indexing with [*] or [number].
   * @param path Must begin with $. e.g $.key1[*].key2[1]
   * @returns all the values of the path specified.
   */
  atPathWithWildcard(path: string /* $.key1[*].key2[1] */): string[] | null;
  /**
   * Type of the value. e.g string, number, boolean, object, array.
   */
  type: string;
  /**
   * Only for objects or arrays.
   * @returns The length of the value.
   */
  length: number;
  /**
   * Returns the value as a string.
   * @returns The value as a string.
   */
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
