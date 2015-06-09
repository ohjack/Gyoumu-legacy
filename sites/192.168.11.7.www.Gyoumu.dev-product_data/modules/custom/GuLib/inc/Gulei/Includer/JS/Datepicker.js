function Datepicker(sel) {
  var _this = this;
  var $this = $(sel);

  $this.find('input.form-text[widget="datepicker"]').each(function() {
    var ele = $(this);

    if (!ele.attr('readonly')) {
      ele.attr('autocomplete', 'off');
      ele.datepicker({
        duration : 0,
        dateFormat : 'yy-mm-dd'
      });
    }
  });

  $this.find('input.form-text[widget="datetimepicker"]').each(function() {
    var ele = $(this);

    if (!ele.attr('readonly')) {
      ele.attr('autocomplete', 'off');
      ele.datetimepicker({
        duration : 0,
        showSecond : true,
        dateFormat : 'yy-mm-dd',
        timeFormat : 'hh:mm:ss',
        stepMinute : 15,
        stepSecond : 15
      });
    }
  });
}
