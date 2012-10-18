<?php


class ContainerServer {
  protected $sites = array();
  protected static $GARDENS_BOOTSTRAP_SUCCESS = 1;
  protected static $GARDENS_BOOTSTRAP_FAIL = 0;

  public function siteStatus($site) {
    $st = $this->siteDrush($site, 'st'); 
    if ($st['status'] === 0) {
      foreach ($st['output'] as $line) {
        if (preg_match('/bootstrap.*Successful/', $line)) {
          return self::$GARDENS_BOOTSTRAP_SUCCESS;
        }
      }
    }
    return self::$GARDENS_BOOTSTRAP_FAIL;
  }

  public function siteDrush($site, $cmd) {
    $exec_command = 'drush --uri=' . $site->dirname . ' ' . $cmd;
    return $this->shellCommand($exec_command);
  }

  public function createSite($site) {
    if (!in_array($site, $this->sites())) {
      mkdir('sites/' . $site->dirname);
      mkdir('sites/' . $site->dirname . '/files');
      chmod('sites/' . $site->dirname . '/files', 0755);
      $settings = file_get_contents('sites/default/default.settings.php');
      $settings_filepath = 'sites/' . $site->dirname . '/settings.php';
      file_put_contents($settings_filepath, $settings);
      chmod($settings_filepath, 0755);
      $db_name = substr(preg_replace('/[^a-zA-Z1-9]/', '_', $site->dirname), 0, 15);
      $mysql_cmd = 'mysql -uroot -e "create database ' . $db_name . ';'
        . 'use ' . $db_name . ';'
        . 'grant all on  ' . $db_name . ' to ' . $db_name . '@\'localhost\' identified by \'' . $db_name . '\';'
        . 'flush privileges;"';
      file_put_contents('sites/' . $site->dirname . '/container.php', "<?php\n\$container_db = '$db_name';");

      drupal_set_message("Site: {$site->dirname} created");
      return TRUE;
    }
    else {
      drupal_set_message("Site: {$site->dirname} already exists", 'error');
      return FALSE;
    }
  }

  public function shellCommand($cmd) {
    $descriptorspec = array(
       0 => array('pipe', 'r'), // Stdin
       1 => array('pipe', 'w'), // Stdout
       2 => array('pipe', 'w'), // Stderr
    );

    $env = array();

    $process = proc_open($cmd, $descriptorspec, $pipes, DRUPAL_ROOT, $env);

    if (is_resource($process)) {
      return array(
        'output' => explode("\n", stream_get_contents($pipes[1])),
        'error'=> explode("\n", stream_get_contents($pipes[2])),
        'status'=> proc_close($process),
      );
    }
  }

  public function sites($list = FALSE) {
    if (!empty($this->sites)) {
      return $this->sites;
    }

    $this->sites = array();

    $files = scandir('sites');
 
    foreach ($files as $file) {
      if (substr($file,0,1) != '.' && is_dir('sites/' . $file) && $file != 'all' && $file != 'default') {
        $this->sites[] = $list ? $file : new ContainerSite($file);
      }
    }

    return $this->sites;
  }
}
