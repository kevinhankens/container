<?php

function container_drush_command() {
  return array(
    'container-task' => array(
      'aliases' => array('ctask'),
      'arguments' => array(
        'action' => dt('init|status|kill|exec'),
      ),
      'options' => array(
        'name' => dt('The name of the task.'),
        'command' => dt('The drush task to complete.'),
        'sites' => dt('A comma-separated list of sites to act on.'),
      ),
    ),
  );
}

/**
 *
 */
function drush_container_task($action) {
  $server = new ContainerServer();
  $name = drush_get_option('name');
  $command = drush_get_option('command');
  $sites = drush_get_option('sites');
  $sites = !empty($sites) ? explode(',', $sites) : $server->sites(TRUE);
  var_dump(container_task($action, $name, $command, $sites));
  return '';
}
