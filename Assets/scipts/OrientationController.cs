using UnityEngine;

public class HandOrientationController : MonoBehaviour
{
    public SerialReader serialReader; // Assign your SerialReader component
    public Transform handTransform;   // The hand model to rotate

    // Optional: smooth rotation
    public float smoothSpeed = 5f;
    private Quaternion targetRotation;

    void Update()
    {
        if (serialReader == null || handTransform == null) return;

        // Get angles from SerialReader
        float Gz = serialReader.Gx;
        float Gx = -serialReader.Gy;
        float Gy = -serialReader.Gz;

        // Convert Euler angles from Arduino to Unity Quaternion
        // Adjust axes if necessary depending on model orientation
        targetRotation = Quaternion.Euler(Gx, Gy, Gz);

        // Smoothly rotate the hand
        handTransform.rotation = Quaternion.Slerp(handTransform.rotation, targetRotation, Time.deltaTime * smoothSpeed);
    }
}
