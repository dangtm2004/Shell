
/***************
 *Dang Truong
 *COSC-2436.S70
 *Lab 2
 *08/02/2018
 ****************/
public class ArraySetDemo
{
	public static void main(String[] args) 
	{
		String[] contentsOfSet = {"A", "A", "B", "A", "C", "A"};

      // Tests on an empty set
      SetInterface<String> aSet = new ResizableArraySet<>(contentsOfSet.length);
      System.out.println("Testing an initially empty bag:");
      testIsEmpty(aSet, true);
      String[] testStrings1 = {"", "B"};
      //testFrequency(aSet, testStrings1);
      testContains(aSet, testStrings1);
      testRemove(aSet, testStrings1);

      // Adding strings
      System.out.println("Adding " + contentsOfSet.length +
                         " strings to an initially empty set with" +
                         " the capacity to hold more than " +
                         contentsOfSet.length + " strings:");
		testAdd(aSet, contentsOfSet);
      
      // Tests on a bag that is not empty
      testIsEmpty(aSet, false);
      String[] testStrings2 = {"A", "B", "C", "D", "Z"};
      //testFrequency(aSet, testStrings2);
      testContains(aSet, testStrings2);
		
      // Removing strings
		String[] testStrings3 = {"", "B", "A", "C", "Z"};
      testRemove(aSet, testStrings3);

		System.out.println("\nClearing the set:");
		aSet.clear();
      testIsEmpty(aSet, true);
		displaySet(aSet);
      
      // Filling an initially empty bag to capacity
      System.out.println("\nTesting an initially empty set that " +
                         " will be filled to capacity:");
		aSet = new ResizableArraySet<String>(7);
		String[] contentsOfBag2 = {"A", "B", "A", "C", "B", "C", "D"};
		testAdd(aSet, contentsOfBag2);
      
      System.out.println("Try to add another string to the full set:");
      if (aSet.add("another string"))
         System.out.println("The set's capacity is resizable to add more strings.");
      else
         System.out.println("The set's capacity is not resizable to add another string: ERROR");
      
      //Test getCurrentSize method
      System.out.println("\nTest getCurrentSize method");
      displaySet(aSet);
      System.out.println ("Current size of the set is: " + aSet.getCurrentSize());
	} // end main
	
   // Tests the method add.
	private static void testAdd(SetInterface<String> aSet, String[] content)
	{
		System.out.print("Adding ");
		for (int index = 0; index < content.length; index++)
		{
			aSet.add(content[index]);
         System.out.print(content[index] + " ");
		} // end for
      System.out.println();
      
		displaySet(aSet);
	} // end testAdd

   // Tests the two remove methods.
	private static void testRemove(SetInterface<String> aSet, String[] tests)
	{
      for (int index = 0; index < tests.length; index++)
      {
         String aString = tests[index];
         if (aString.equals("") || (aString == null))
         {
            // test remove()
            System.out.println("\nRemoving a string from the set:");
            String removedString = aSet.remove();
            System.out.println("remove() returns " + removedString);
         }
         else
         {
            // test remove(aString)
            System.out.println("\nRemoving \"" + aString + "\" from the set:");
            boolean result = aSet.remove(aString);
            System.out.println("remove(\"" + aString + "\") returns " + result);
         } // end if
         
         displaySet(aSet);
      } // end for
	} // end testRemove

   // Tests the method isEmpty.
   // correctResult indicates what isEmpty should return.   
	private static void testIsEmpty(SetInterface<String> aSet, boolean correctResult)
	{
      System.out.print("Testing isEmpty with ");
      if (correctResult)
         System.out.println("an empty Set:");
      else
         System.out.println("a Set that is not empty:");
      
      System.out.print("isEmpty finds the Set ");
      if (correctResult && aSet.isEmpty())
			System.out.println("empty: OK.");
		else if (correctResult)
			System.out.println("not empty, but it is empty: ERROR.");
		else if (!correctResult && aSet.isEmpty())
			System.out.println("empty, but it is not empty: ERROR.");
		else
			System.out.println("not empty: OK.");      
		System.out.println();
	} // end testIsEmpty
	
/***
   // Tests the method getFrequencyOf.
	private static void testFrequency(SetInterface<String> aSet, String[] tests)
	{
 		System.out.println("\nTesting the method getFrequencyOf:");
      for (int index = 0; index < tests.length; index++)
      {
         String aString = tests[index];
         if (!aString.equals("") && (aString != null))
         {
            System.out.println("In this bag, the count of " + tests[index] +
                               " is " + aSet.getFrequencyOf(tests[index]));
         } // end if
      } // end for
   } // end testFrequency
   
***/

   // Tests the method contains.
	private static void testContains(SetInterface<String> aSet, String[] tests)
	{
 		System.out.println("\nTesting the method contains:");
      for (int index = 0; index < tests.length; index++)
      {
         String aString = tests[index];
         if (!aString.equals("") && (aString != null))
         {
            System.out.println("Does this set contain " + tests[index] + 
                               "? " + aSet.contains(tests[index]));
         } // end if
      } // end for
   } // end testContains

   // Tests the method toArray while displaying the set.
	private static void displaySet(SetInterface<String> aSet)
	{
		System.out.println("The set contains " + aSet.getCurrentSize() +
		                   " string(s), as follows:");		
		Object[] bagArray = aSet.toArray();
		for (int index = 0; index < bagArray.length; index++)
		{
			System.out.print(bagArray[index] + " ");
		} // end for
		
		System.out.println();
	} // end displaySet
} // end ArraySetDemo

