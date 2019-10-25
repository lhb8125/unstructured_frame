/**
* @file: parameter.cpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-09-20 14:17:19
* @last Modified by:   lenovo
* @last Modified time: 2019-09-24 09:06:07
*/
#include "parameter.hpp"
extern Domain *dom;
/**
* @brief Control parameter
*/
class Parameter
{
private:
	string paraFile_;
public:
	/**
	* @brief default constructor
	*/
	Parameter(const string paraFile){ paraFile_ = paraFile; };
	/**
	* @brief deconstructor
	*/
	~Parameter();
	/**
	* @brief operator overloading to struct Equation
	*/
	void operator >> (const YAML::Node& node, Equation& equ) {
	   node["name"] >> equ.name;
	   node["solver"] >> equ.solver;
	   node["preconditioner"] >> equ.preconditioner;
	};
	/**
	* @brief operator overloading to struct Scheme
	*/
	void operator >> (const YAML::Node& node, Scheme& scheme) {
	   node["name"] >> scheme.name;
	   node["scheme"] >> scheme.scheme;
	};
	/**
	* @brief operator overloading to struct Turbulent
	*/
	void operator >> (const YAML::Node& node, Turbulent& turb) {
	   node["model"] >> turb.model;
	};
	/**
	* @brief operator overloading to struct Solve
	*/
	void operator >> (const YAML::Node& node, Solve& slv) {
	   node["deltaT"] >> slv.deltaT;
	   node["startT"] >> slv.startT;
	   node["endT"] >> slv.endT;
	   node["writeInterval"] >> slv.writeInterval;
	};	
	/**
	* @brief operator overloading to struct Region
	*/
	void operator >> (const YAML::Node& node, Region& reg) {
	   node["name"] >> reg.name;
	   node["path"] >> reg.path;
	   const YAML::Node& scheme = node["scheme"];
	   reg.scheme = new Scheme(scheme.size());
	   for (int i = 0; i < scheme.size(); ++i)
	   {
	       scheme[i] >> reg.scheme[i];
	   }
	};
	/**
	* @brief read parameter through yaml-cpp
	*/
	void readPara()
	{
		std::ifstream fin(paraFile_);
		YAML::Parser parser(fin);
		YAML::Node doc;
		parser.GetNextDocument(doc);
		dom = new Domain(doc.size());
		for(int i=0;i<doc.size();i++)
		{
			const YAML::Node& equation = doc[i]["equation"];
			dom[i].equation = new Equation(equation.size());
			for (int j = 0; j < equation.size(); ++j)
			{
				equation[i] >> dom[i].equation;
			}

			const YAML::Node& region = doc[i]["region"];
			dom[i].region = new Region(region.size());
			for (int j = 0; j < region.size(); ++j)
			{
				region[i] >> dom[i].region;
			}

			const YAML::Node& turbulent = doc[i]["turbulent"];
			turbulent >> dom[i].turbulent;

			const YAML::Node& solve = doc[i]["solver"];
			solve >> dom[i].solver;
		}
	};	
};

getDomain("domain1").getRegion("region1").getPath();
getDomain("domain1").getTurbulent().getModel();