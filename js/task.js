(function($){

  Drupal.behaviors.container = {
    attach: function(context, settings) {
      // Take over the drush command form.
      $('#edit-drush-submit').unbind().bind('click', function(e) {
        e.preventDefault(); 
        Drupal.container.task.init();
      });
    }
  };

  Drupal.container = {};

  /**
   * The task object handles initiating, killing or checking
   * the status of any tasks that the server is responsible for.
   */
  Drupal.container.task = {
    name: new Date().getTime(),
    /**
     *
     */
    init: function() {
      var sites = [];
      $('.container_site_name').each(function(id, item) {
        sites.push($(item).attr('data-site'));
      });

      var task = $('#edit-drush-command').val();
 
      var success = function(data, textStatus, jqXHR) {
        Drupal.container.task.status = Drupal.settings.container.GARDENS_TASK_UNKNOWN;
        // Check status via ajax calls until it's complete
        console.log(data);
        $('#drush-command-output').before('<div id="drush-progress" style="border:1px solid #333;height:10px;background:top left repeat-x url(/misc/progress.gif);"></div>');
        Drupal.container.task.interval = setInterval(Drupal.container.task.stat, 5000);
      }
      var error = function(data, textStatus, jqXHR) {
        console.log(data);
      }

      this.send('init', task, sites, success, error);
    },
    /**
     *
     */
    kill: function() {
    },
    /**
     *
     */
    stat: function() {
      Drupal.container.task.status = Drupal.settings.container.GARDENS_TASK_UNKNOWN;
      var success = function(data, textStatus, jqXHR) {
        if (typeof data != 'undefined') {
          Drupal.container.task.status = data.status;
          $('#drush-command-output').html(data.output);
          if (data.status == Drupal.settings.container.GARDENS_TASK_COMPLETE || data.status == Drupal.settings.container.GARDENS_TASK_ERROR) {
            clearInterval(Drupal.container.task.interval);
            $('#drush-command-output').append('<div class="message">Finished</div>');
            Drupal.container.task.name += 1;
            $('#drush-progress').remove();
          }
        }
      }
      var error = function(data, textStatus, jqXHR) {
        console.log(data);
      }
      Drupal.container.task.send('stat', null, null, success, error);
    },
    /**
     *
     */
    send: function(action, task, site_list, on_success, on_error) {
      $.ajax({
        url: '/container/task/' + action,
        success: on_success,
        error: on_error,
        type: 'POST',
        dataType: 'json',
        data: {
          sites: site_list,
          command: task,
          name: Drupal.container.task.name,
        },
      });
    }
  };

})(jQuery);
