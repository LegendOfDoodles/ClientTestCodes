using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;




public class TranformExporter : MonoBehaviour {
    public GameObject[] Objects;
    public GameObject[] Waypoint;
    public GameObject[] NexusTower;
    public GameObject[] Monster;

    string m_strPath = "/Resources/";

	// Use this for initialization
	void Start () {
        WriteObjectData();
        WriteWayPointData();
        WriteNecusTowerData();
        MonsterData();
    }
    public void MonsterData()
    {
        DirectoryInfo dtif = new DirectoryInfo(m_strPath);
        if (!dtif.Exists)
        {
            dtif.Create();
        }
        FileInfo file = new FileInfo(Application.dataPath + m_strPath + "MonsterSetting.txt");
        if (!file.Exists)
        {
            FileStream fs = file.Create();
            fs.Close();
        }

        FileStream fileStream = file.OpenWrite();

        TextWriter tw = new StreamWriter(fileStream);
        for (int i = 0; i < Monster.Length; ++i)
        {
            Transform[] tempTransforms = Monster[i].GetComponentsInChildren<Transform>();
            tw.WriteLine(Monster[i].name);
            tw.WriteLine(tempTransforms.Length - 1);
            int n = 0;
            foreach (Transform child in tempTransforms)
            {
                if (n != 0)
                {

                    tw.WriteLine(child.position.x + " " + child.position.y + " " + child.position.z);
                    tw.WriteLine(child.rotation.eulerAngles.x + " " + child.rotation.eulerAngles.y + " " + child.rotation.eulerAngles.z);
                    tw.WriteLine(child.lossyScale.x + " " + child.lossyScale.y + " " + child.lossyScale.z);
                }
                n++;
            }
            tw.WriteLine("<end>");
        }
        tw.Close();
        fileStream.Close();

        Debug.Log("succese-Monster");

    }
    public void WriteNecusTowerData()
    {
        DirectoryInfo dtif = new DirectoryInfo(m_strPath);
        if (!dtif.Exists)
        {
            dtif.Create();
        }
        FileInfo file = new FileInfo(Application.dataPath + m_strPath + "NexusTowerSetting.txt");
        if (!file.Exists)
        {
            FileStream fs = file.Create();
            fs.Close();
        }

        FileStream fileStream = file.OpenWrite();

        TextWriter tw = new StreamWriter(fileStream);
        for (int i = 0; i < NexusTower.Length; ++i)
        {
            Transform[] tempTransforms = NexusTower[i].GetComponentsInChildren<Transform>();
            tw.WriteLine(NexusTower[i].name);
            tw.WriteLine(tempTransforms.Length - 1);
            int n = 0;
            foreach (Transform child in tempTransforms)
            {
                if (n != 0)
                {

                    tw.WriteLine(child.position.x + " " + child.position.y + " " + child.position.z);
                    tw.WriteLine(child.rotation.eulerAngles.x + " " + child.rotation.eulerAngles.y + " " + child.rotation.eulerAngles.z);
                    tw.WriteLine(child.lossyScale.x + " " + child.lossyScale.y + " " + child.lossyScale.z);
                }
                n++;
            }
            tw.WriteLine("<end>");
        }
        tw.Close();
        fileStream.Close();

        Debug.Log("succese-NexusTower");

    }
    public void WriteObjectData()
    {
        DirectoryInfo dtif = new DirectoryInfo(m_strPath);
        if (!dtif.Exists)
        {
            dtif.Create();
        }
        FileInfo file = new FileInfo(Application.dataPath+ m_strPath + "ObjectSetting.txt");
        if (!file.Exists)
        {
            FileStream fs = file.Create();
            fs.Close();
        }

        FileStream fileStream = file.OpenWrite();

        TextWriter tw = new StreamWriter(fileStream);
        for (int i=0; i<Objects.Length; ++i)
        {
            Transform[] tempTransforms = Objects[i].GetComponentsInChildren<Transform>();
            tw.WriteLine(Objects[i].name);
            tw.WriteLine(tempTransforms.Length - 1);
            int n = 0;
            foreach (Transform child in tempTransforms)
            {
                if (n != 0)
                {

                tw.WriteLine(child.position.x + " "+ child.position.y + " " + child.position.z);
                tw.WriteLine(child.rotation.eulerAngles.x + " "+ child.rotation.eulerAngles.y + " " + child.rotation.eulerAngles.z);
                tw.WriteLine(child.lossyScale.x + " "+ child.lossyScale.y + " " + child.lossyScale.z );
                }
                n++; 
            }
            tw.WriteLine("<end>");
        }
        tw.Close();
        fileStream.Close();

        Debug.Log("succese-Object");

    }
    public void WriteWayPointData()
    {
        DirectoryInfo dtif = new DirectoryInfo(m_strPath);
        if (!dtif.Exists)
        {
            dtif.Create();
        }
        FileInfo file = new FileInfo(Application.dataPath+ m_strPath + "WayPointSetting.txt");
        if (!file.Exists)
        {
            FileStream fs = file.Create();
            fs.Close();
        }

        FileStream fileStream = file.OpenWrite();

        TextWriter tw = new StreamWriter(fileStream);
        for (int i = 0; i < Waypoint.Length; ++i)
        {
            Transform[] tempTransforms = Waypoint[i].GetComponentsInChildren<Transform>();
            tw.WriteLine(Waypoint[i].name);
            tw.WriteLine(tempTransforms.Length - 1);
            int n = 0;
            foreach (Transform child in tempTransforms)
            {
                if (n != 0)
                {

                    tw.WriteLine(child.position.x + " " + child.position.y + " " + child.position.z);
                    tw.WriteLine(child.rotation.eulerAngles.x + " " + child.rotation.eulerAngles.y + " " + child.rotation.eulerAngles.z);
                    tw.WriteLine(child.lossyScale.x + " " + child.lossyScale.y + " " + child.lossyScale.z);
                }
                n++;
            }
            tw.WriteLine("<end>");
        }
        tw.Close();
        fileStream.Close();

        Debug.Log("succese-Way");

    }

    // Update is called once per frame
    void Update () {

		
	}
}
