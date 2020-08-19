First Code Challenge
====================

This is the first challenge in your Advanced Programming Class.

1. Solve the [Length of a Nested Array](https://edabit.com/challenge/yXSTvCNen2DQHyrh6) challenge by using the programming language of your choice.
2. Below are some documentation sections that need to be updated. Those are required for the processor to build and execute your code.
3. Certainly, this problem is already solved in the web, so we recommend you to solve it by your own without consulting others solutions.
4. This is a exploratory excersice that will help the professor to adapt the labs and challenges for the course.
3. Have fun and come prepared on next class.

Build Requirements
------------------

- JavaScript knowledge
- Web Browser
- Platform to run your code, the next one is a good option to observe live changes:
https://playcode.io/



How to build
------------

Is pretty importan to acclarify that in this version the work that we are going to make is to create a new linear array where we can get the length with the function we already have, the difficult part is going to be to get out all the elements on it.

The first part is to create our function:
function straight(array) {

}

Second is to declare the variable that we are going to need, as I said before, we are going to generate a new linear array so we are going to need an empty array, a string structure that we are going to use as and stack to move the elements and an auxiliary variable to make the moves:
var temp;
var result = [];
var stack = array.slice();
var stringArray = '[object Array]';

After that we have a procedure where at the beginning we are going to evaluate that is not an empty array the one that we get as parameter and after that we are going to relocate one element from the array and store it in our variable temp to then put it in the new array, this is a process that runs through a while loop until there are no more elements to add to the result array:
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

At the end depending the use we decide if we need it only to prompt on the screen or return the value
console.log(result.length);
//return result.length;



How to Run
----------

Testing this program is pretty easy, the only thing that we need to do is to create a nested array and call the function with this array as parameter. In the link of requirements can be tested
var array = [[0, 1], [2, 3], [4, 5, [6, 7, [8, [9, 10]]]],11,12,13,[[14,[15,16]],[17,18]]];
straight(array);



General instructions
--------------------
1. Don't forget to sync first with the base [master](https://github.com/CodersSquad/ap-labs) branch.
2. Push your code to your personal fork.
3. Don't forget to update this `README.md` file with your project's `requirements`, `build` and `how to run` sections.
4. Have fun and don't forget the next section.


How to submit your work
=======================

GITHUB_USER=<your_github_user>  make submit

More details at: [Classify API](../../classify.md)
