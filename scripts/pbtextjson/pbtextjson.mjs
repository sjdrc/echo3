#!/bin/node
import { parse } from "pb-text-format-to-json";
import fs from "fs";
import path from "path";
import camelCase from 'camelcase';
import { stringify } from "querystring";

var formats = [];

formats[".ematerial"] = {
	shouldBeArrays: [
		"pass",
		"textureUnit",
		"frame",
		"geometryShader",
		"vertexShader",
		"fragmentShader",
		"variable",
		"material"
	],
	shouldNotBeArrays:[],
	noCamelCaseFieldValues: [
		"image",
		"name",
		"vertex_shader",
		"fragment_shader",
		"geometry_shader",
		"variable"
	]
};

formats[".font"] = {
	shouldBeArrays: [
		"range",
		"characters"
	],
	shouldNotBeArrays: [
	],
	noCamelCaseFieldValues: [
		"filename",
		"name"
	]
};

formats[".materialBundle"] = {
	shouldBeArrays: formats[".ematerial"].shouldBeArrays,
	shouldNotBeArrays: formats[".ematerial"].shouldNotBeArrays,
	noCamelCaseFieldValues: formats[".ematerial"].noCamelCaseFieldValues,
};

formats[".input"] = {
	shouldBeArrays: [
		"analogue.source",
		"value",
		"analogue",
		"digital"
	],
	shouldNotBeArrays: [
	],
	noCamelCaseFieldValues: [
		"source",
		"name"
	]
};

formats[".gui"] = {
	shouldBeArrays: [
		"function",
		"enterFunction",
		"exitFunction",
		"hoverFunction",
		"leaveFunction",
		"pressedFunction",
		"holdFunction",
		"releasedFunction",
		"element",
		"guiFile",
		"propery",
		"conditional"
		],
	shouldNotBeArrays: [
		"cursor.element"
	],
	noCamelCaseFieldValues: [
		"text",
		"font",
		"name",
		"idle_material",
		"pressed_material",
		"released_material",
		"hover_material",
		"enter_function",
		"hover_function",
		"leave_function",
		"pressed_function",
		"hold_function",
		"released_function",
		"condition",
		"physical_width",
		"physical_height",
		"colour",
		"property",
		"font",
		"material",
		"x_axis",
		"y_axis",
		"button"
	]
};

const verbose = true;
let converted = 0;
let skipped = 0;
let failed = 0;
const stringifySpaces = 2;

function ShouldBeArray(convertContext,key,parentKey)
{
	return (convertContext.shouldBeArrays.includes(key) || convertContext.shouldBeArrays.includes(parentKey+"."+key)) &&
		!(convertContext.shouldNotBeArrays.includes(key) || convertContext.shouldNotBeArrays.includes(parentKey+"."+key));
}

function EnsureArraysAreCorrectRecursively(convertContext,o,parentKey = "", d=0)
{
	// recursively enter asObject and make sure anything named in shouldBeArrays is an array with the content.
	for (var key in o)
	{
		//console.log(parentKey,key,d);
		if (typeof o[key] === 'object')
		{
			EnsureArraysAreCorrectRecursively(convertContext,o[key],Array.isArray(o) ? parentKey : key,++d);
			if(ShouldBeArray(convertContext,key,parentKey) && !Array.isArray(o[key]))
			{
				o[key] = [ o[key] ];
			}
		} else if (o.hasOwnProperty(key))
		{
			if(ShouldBeArray(convertContext,key,parentKey))
			{
				o[key] = [ o[key] ];
			}
		}
	}
	return o;
}

function EnsureCamelCase(convertContext,o)
{
	let newObj;
	if(Array.isArray(o))
	{
		newObj = [];
	}else
	{
		newObj = {};
	}
	for (var key in o)
	{
		let ccKey;
		if(typeof key === 'string')
		{
			ccKey = camelCase(key);
		}else
		{
			ccKey = key;
		}
		if (typeof o[key] === 'object')
		{
			newObj[ccKey] = EnsureCamelCase(convertContext,o[key]);
		} else if (o.hasOwnProperty(key))
		{
			// Convert value
			if(typeof o[key] === 'string' && !convertContext.noCamelCaseFieldValues.includes(key) && !Array.isArray(o))
			{
				newObj[ccKey] = camelCase(o[key]);
			}else
			{
				newObj[ccKey] = o[key];
			}
		}
	}
	return newObj;

}

function RemoveSemicolons(input)
{
	let lines = input.split('\n');
	let output = "";
	for(let i=0;i<lines.length;++i)
	{
		output += lines[i].replace(/^;+|;+\r*\n*/g, '') + "\n";
	}
	return output;
}

function ConvertPBTextToJSON(convertContext,fileName)
{
	const pbFileName = fileName+".pb";
	if(fs.existsSync(pbFileName))
	{
		if(verbose)
		{
			console.log("Skipping " + fileName + " since .pb exists");
		}
		skipped++;
		return;
	}
	if(verbose)
	{
		console.log("Renaming: " + fileName + " to " + pbFileName);
	}
	fs.renameSync(fileName,pbFileName);
	if(verbose)
	{
		console.log("Reading pb file: " + pbFileName);
	}
	let input = fs.readFileSync(pbFileName, "utf-8");
	input = RemoveSemicolons(input);
	let asObject = parse(input);
	if ('error' in asObject)
	{
		console.log("Reverting: " + pbFileName + " to " + fileName);
		fs.renameSync(pbFileName,fileName);
		failed++;
		return;
	}
	// console.log("Before CC");
	// console.log(JSON.stringify(asObject,null,2));
	asObject = EnsureCamelCase(convertContext,asObject);
	//console.log("After CC");
	//console.log(JSON.stringify(asObject,null,2));
	EnsureArraysAreCorrectRecursively(convertContext,asObject);
	//console.log("After Array");
	//console.log(JSON.stringify(asObject,null,2));
	const output = JSON.stringify(asObject,null,stringifySpaces);
	fs.writeFileSync(fileName, output)
	console.log("Converted: " + fileName);
	converted++;
}

function RevertPath(fileName)
{
	const pbFileName = fileName+".pb";
	if(fs.existsSync(pbFileName))
	{
		console.log("Restoring file",fileName,"from",pbFileName);
		fs.rmSync(fileName);
		fs.renameSync(pbFileName,fileName);
		converted++;
	}else
	{
		console.log("Failed to restore file",fileName,"from",pbFileName);
	}
}

function ProcessPath(pathName, processor, depth = "")
{
	const stats = fs.statSync(pathName);
	if(stats.isDirectory())
	{
		if(verbose)
		{
			console.log("Scanning folder: " + pathName);
		}
		const fileObjs = fs.readdirSync(pathName);
		fileObjs.forEach(folderFile => {
			ProcessPath(pathName+"/"+folderFile,processor,depth+" ");
		});
	}else
	{
		const ext = path.extname(pathName);
		if(formats.hasOwnProperty(ext))
		{
			if(verbose)
			{
				console.log(depth + "File: " + pathName);
			}
			processor(formats[ext],pathName);
		}
	}
}

if(process.argv.length>2)
{
	//console.log(formats);
	if(process.argv[2]=="--revert")
	{
		const inputFiles = process.argv.slice(3);
		let pbPaths = [];
		let buildPbPaths = function(convertContex,fileName){
			const pbFileName = fileName+".pb";
			if(fs.existsSync(pbFileName))
			{
				pbPaths.push(fileName);
			};
		};
		inputFiles.forEach(function (val, index, array) {
			ProcessPath(val,buildPbPaths);
		});
		for(let i=0;i<pbPaths.length;++i)
		{
			RevertPath(pbPaths[i]);
		}
		
	}else
	{
		const inputFiles = process.argv.slice(2);
		inputFiles.forEach(function (val, index, array) {
			ProcessPath(val,ConvertPBTextToJSON);
		});

		// let o = { pass: {a:1,b:2,pass:"another"}, thing: {a:10}};
		// console.log(JSON.stringify(o));
		// EnsureArraysAreCorrectRecursively(o);
		// console.log(JSON.stringify(o));

		console.log("Converted:", converted);
		console.log("Skipped:", skipped);
		console.log("Failed:",failed);
	}
}
