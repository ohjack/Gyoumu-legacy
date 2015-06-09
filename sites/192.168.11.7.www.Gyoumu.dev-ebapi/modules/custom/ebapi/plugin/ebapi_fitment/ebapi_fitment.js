Drupal.behaviors.ebapiFitmentSelector = function(){
  var $this = this;
  var CompList = $('input#edit-selected');

  this.getCompaList = function(){
    return JSON.parse(CompList.val());
  }

  this.setCompaList = function(send){
    CompList.val(JSON.stringify(send));
  }

  $('select.ebapi-fitment-selector').change(function(){
    var changed = $(this);
    var form_item = changed.parents('div.form-item:eq(0)');

    if(changed.hasClass('year')){
      var filters = form_item.nextAll().find('select.ebapi-fitment-selector').not('.no-year-dep');
    }
    else {
      var filters = form_item.nextAll().find('select.ebapi-fitment-selector');
    }

    filters.find('option').attr('selected', '');
    filters.find('option[value="0"]').attr('selected', 'selected');
  });

  $('input#edit-add-fitment').mousedown(function(){
    var send = $this.getCompaList();

    $('div#ebapi-fitment-list :checked').not(':disabled').each(function(){
      var each = $(this);
      send[each.attr('value')] = each.parent().text();
    });

    $this.setCompaList(send);
  });

  $('input#edit-del-fitment').mousedown(function(){

    var send = $this.getCompaList();

    $('div#ebapi-compatibility-list :checked').each(function(){
      var key = $(this).attr('value');

      if(send[key] !== undefined){
        delete send[key];
      }
    });

    $this.setCompaList(send);
  });

  $('div.form-item span.toggle-all').click(function(){
    var toggle = $(this);

    if(toggle.hasClass('on')) {
      toggle.parents('div.form-item:eq(0)').find(':checkbox').not(':disabled"').attr('checked', '');
      toggle.removeClass('on');
    }
    else{
      toggle.parents('div.form-item:eq(0)').find(':checkbox').attr('checked', 'checked');
      toggle.addClass('on');
    }
  });

  $('input#edit-update-fitment').mousedown(function(){
    if($('div#ebapi-listings-list td input:checked').length < 1){
      alert('No active listing selected.');
      return false;
    }

    var sels = $('select.ebapi-fitment-selector');
    sels.find('option').attr('selected', '');
    sels.find('option[value="0"]').attr('selected', 'selected');

    $('div#ebapi-compatibility-list :checkbox').attr('checked', '');
  });

  $('div.ebapi-searchpanel-search :text').keydown(function(e){
    if(e.keyCode == 13){
     $('#edit-search-search').mousedown();
      return false;
    }
  });

  var ImportTrigger = $('div#ebapi-listings-list table.gu-table td[gu_name="ItemID"] span.gu_value');
  ImportTrigger.not('.processed').click(function(){
    var $this = $(this);
    var ItemID = $this.text();

    $('em.list-empty').remove();
    $('div#ebapi-compatibility-list div.form-checkboxes')
        .html('<em class="msg-importing">Importing from ' + ItemID + '...</em>');

    $.post(
      '?q=ebapi/fitment/import/js',
        {
          'ItemID' : ItemID,
          'accnt' : $('input#edit-accnt').val(),
          'site': $('input#edit-site').val()
        },
        function(data){
          $('input#edit-selected').val(data);
          $('select.ebapi-fitment-selector:eq(0)').change();
        }
    );
  });

  ImportTrigger.addClass('processed');
}
