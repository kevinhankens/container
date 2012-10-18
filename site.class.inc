<?php

class ContainerSite {

  protected $info = array();

  public function __construct($name = '') {
    if (!empty($name)) {
      $this->info['dirname'] = $name;
    }
  }

  public function __set($key, $val) {
    $this->info[$key] = $val;
  }

  public function __get($key) {
    return $this->info[$key];
  }

}
