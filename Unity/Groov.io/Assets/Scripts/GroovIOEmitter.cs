using UnityEngine;
using System.Collections;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public struct groov_config_t {
  public string host_name;
  public int host_port;
}


public class GroovIOEmitter : MonoBehaviour {

  [DllImport("Assets/Plugins/GroovIO/GroovIO.so")]
  public extern static void groov_init(ref groov_config_t config);

  [DllImport("Assets/Plugins/GroovIO/GroovIO.so")]
  public extern static void groov_connect();

  [DllImport("Assets/Plugins/GroovIO/GroovIO.so")]
  public extern static void groov_run_loop_step();

  public groov_config_t config = new groov_config_t();

  // Use this for initialization
  void Awake () {
    config.host_name = "localhost";
    config.host_port = 3000;
    groov_init(ref config);
  }

  void Start() {
    groov_connect();
  }
  
  // Update is called once per frame
  void Update () {
    groov_run_loop_step();
  }
}
