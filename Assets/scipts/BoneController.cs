using UnityEngine;

public class BoneController : MonoBehaviour
{
    public Transform rootBone;
    public Transform[] extraBones;
    public SerialReader serialReader; // Reference to the SerialReader script



    void Update()
    {
        // Read the latest sensor value
        float sensorValue = serialReader.angle;
        Debug.Log("Sensor Value: " + sensorValue);
        // Apply rotation
        RotateFinger(sensorValue);
    
    } 

    void RotateFinger(float angle)
    {
        // Apply rotation to root and child bones
        rootBone.localRotation = Quaternion.Euler(angle, 0f, 0f);

        foreach (Transform bone in extraBones)
            bone.localRotation = Quaternion.Euler(angle, 0f, 0f);
    }

}
