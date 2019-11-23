using System;
using System.Collections.Generic;
public class Ass1 {
	static List<string> tree(int n) {
		if (n == 1) {
			List<string> s = new List<string>();
			s.Add("(.)");
			return s;
		}

		List<string> strings = new List<string>();
		List<string> prev = tree(n - 1);
		foreach (string T in prev) {
			strings.Add("(" + T + ".)");
	        strings.Add("(." + T + ")");
		}
		return strings;
	}

    static void Main() {
        Console.Write("Enter the number of nodes: ");
        List<string> treeList = tree(Convert.ToInt32(Console.ReadLine()));
        foreach (string tree in treeList) {
            Console.WriteLine(tree);
        }
    }
}

// mcs -out:hello.exe hello.cs