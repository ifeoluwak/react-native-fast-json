import { useEffect } from 'react';
import { Text, View, StyleSheet } from 'react-native';
import { fastJson } from 'react-native-fast-json';

export default function App() {
  useEffect(() => {
    fastJson.parse('{ "name": "John", "age": 30 }').then((result) => {
      console.log(result?.getValue('name'));
    });
  }, []);

  return (
    <View style={styles.container}>
      <Text>Result</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
});
