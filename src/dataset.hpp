#ifndef INCLUDE_DATASET
#define INCLUDE_DATASET

#include <string>
#include <map>

class Dataset {
	public:
		int size1;
		int size2;
	
		std::map<std::string,int> *indices1;
		std::map<std::string,int> *indices2;

		std::map<int,std::string> *labels1;
		std::map<int,std::string> *labels2;

		double *values;
		double *refValues;

		Dataset ();
		~Dataset ();

		void print ();
		void buildDataset ();
		
		void initValues (double v = 0);
		void initRefValues (double v = 0);

		void addLabel1 (std::string str);
		void addLabel2 (std::string str);

		std::string getLabel1 (int i);
		std::string getLabel2 (int j);

		int getIndex1 (std::string s1);
		int getIndex2 (std::string s2);
		
		double getValue (int i, int j);
		double getRefValue (int i, int j);
		
		double getValue (std::string s1, std::string s2);
		double getRefValue (std::string s1, std::string s2);
		
		void setValue (int i, int j, double v);
		void setRefValue (int i, int j, double v);
		
		void setValue (std::string s1, std::string s2, double v);
		void setRefValue (std::string s1, std::string s2, double v);
		
		void incrementValue (int i, int j);
		void incrementRefValue (int i, int j);
		
		void incrementValue (std::string s1, std::string s2);
		void incrementRefValue (std::string s1, std::string s2);

		double *getValues1 (std::string s2);
		double *getValues2 (std::string s1);
		
		double *getRefValues1 (std::string s2);
		double *getRefValues2 (std::string s1);

		double *getValues (bool order = true);
		double *getRefValues (bool order = true);
};


#endif
