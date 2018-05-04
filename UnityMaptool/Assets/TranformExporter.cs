﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;




public class TranformExporter : MonoBehaviour {
    public GameObject[] Objects;

    string m_strPath = "Assets/Resources/";

	// Use this for initialization
	void Start () {
        WriteData();

    }
    public void WriteData()

    {
        DirectoryInfo dtif = new DirectoryInfo(m_strPath);
        if (!dtif.Exists)
        {
            dtif.Create();
        }
        FileInfo file = new FileInfo(Application.dataPath+"/Setting.txt");
        if (!file.Exists)
        {
            FileStream fs = file.Create();
            fs.Close();
        }

        FileStream fileStream = file.OpenWrite();

        TextWriter tw = new StreamWriter(fileStream);

        for(int i=0; i<Objects.Length; ++i)
        {
            Transform[] tempTransforms = Objects[i].GetComponentsInChildren<Transform>();
            tw.WriteLine(Objects[i].name);
            tw.WriteLine(tempTransforms .Length);
            int n = 0;
            foreach (Transform child in tempTransforms)
            {
                if (n != 0)
                {

                tw.WriteLine(child.position.x + " "+ child.position.y + " " + child.position.z);
                tw.WriteLine(child.rotation.x + " "+ child.rotation.y + " " + child.rotation.z);
                tw.WriteLine(child.lossyScale.x + " "+ child.lossyScale.y + " " + child.lossyScale.z );
                }
                n++; 
            }
        }
        tw.Close();
        fileStream.Close();

        Debug.Log("succese");

    }


	// Update is called once per frame
	void Update () {

		
	}
}