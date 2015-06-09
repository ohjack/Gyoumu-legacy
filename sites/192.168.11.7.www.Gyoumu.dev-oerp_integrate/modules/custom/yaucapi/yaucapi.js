Drupal.behaviors.datepickerController = function() {
  $('input.form-text[widget="datepicker"]').each(function() {
    var $this = $(this);

    $this.attr('autocomplete', 'off');
    $this.datepicker({
      duration : 0,
      dateFormat : 'yy-mm-dd'
    });
  });
}
