var array = [[0, 1], [2, 3], [4, 5, [6, 7, [8, [9, 10]]]],11,12,13,[[14,[15,16]],[17,18]]];
straight(array);
function straight(array) {
var temp;
var result = [];
var stack = array.slice();
var stringArray = '[object Array]';
if (!array.length) {
      return result;
  }
  node = stack.pop();
  do {
      if (toString.call(temp) === stringArray) {
          stack.push.apply(stack, temp);
      } else {
          result.push(temp);
      }
  } while (stack.length && (temp = stack.pop()) !== undefined);
  console.log(result.length);
  //return result.length;
}
