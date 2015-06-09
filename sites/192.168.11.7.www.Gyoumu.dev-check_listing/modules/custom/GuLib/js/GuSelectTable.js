function GuSelTableEvent(ev){
  this.isSticky = false;
  this.ev= $(ev);
  this.table = this.ev.closest('table');
  
//  prepare target table & sticky-header
  if(!this.table.hasClass('gu-seltable')){
    if(this.table.hasClass('sticky-header')){
      this.sticky = this.table;
      this.table = this.table.next('table.gu-seltable');
      
      if(this.table.length == 0){
        return;
      }
      this.isSticky = true;
    }
  }
  else{
    this.sticky = this.table.prev('table.sticky-header');
  }
  
  this.reset = function(){
    this.table.find('td.selector :checkbox').attr('checked', '');
    this.table.find('tr').removeClass('selected');
  }
  
}

function GuSelTableCheck(ev){
  this.prototype = new GuSelTableEvent(ev);
  var $this = this.prototype;
  
  this.toggleAll = function(){
//  check/uncheck all
    if($this.ev.attr('checked')){
      $this.table.find('td.selector input[type="checkbox"]').each(function(){
        var tr = $(this).parents('tr:eq(0)');
        if(tr.css('display') != 'none' && !tr.hasClass('sep-after')) {
          $(this).attr('checked', 'checked').change();
        }
      });
    }
    else{
      $this.table.find('td.selector input[type="checkbox"]').each(function(){
        $(this).attr('checked', '').change();
      });
    }
    
//  sync with sticky-header
    if($this.isSticky){
      $this.table.find('th.selector input[type="checkbox"]')
                 .attr('checked', $this.ev.attr('checked'));
    }
    else{
      $this.sticky.find('th.selector input[type="checkbox"]')
                  .attr('checked', $this.ev.attr('checked'));
    }
  }
}

function GuSelTableFilter(ev){
  this.prototype = new GuSelTableEvent(ev);
  var $this = this.prototype;
  
  this.check = function(){
    $this.reset();
//  check matched
    var gu_name = $this.ev.parent('th').attr('gu_name');
    var val = $this.ev.find('option:selected').val();

    $this.table.find('td.selector input[type="checkbox"]').attr('checked', '');

    $this.table.find('td[gu_name="' + gu_name + '"] span.gu_value').each(function(){
      var each = $(this);

      if(each.text() == val){
        var tr = each.parents('tr:eq(0)');

        if(!tr.hasClass('sep-after')){
          tr.find('td.selector input[type="checkbox"]').attr('checked', 'checked').change();
        }
      }
    });
    
//  sync with sticky-header
    if($this.isSticky){
      var sel = $this.table.find('th[gu_name="' + gu_name + '"] select.GuSelectTable.filter');
      sel.find('option[value="' + val + '"]').attr('selected', 'selected');
    }
    else{
      var sel = $this.sticky.find('th[gu_name="' + gu_name + '"] select.GuSelectTable.filter');
      sel.find('option[value="' + val + '"]').attr('selected', 'selected');
    }
  }
}

function GuSelTablePrompt(){
  var $this = this;
  
  this.init = function(){
    $('div.gu-view div.toolbar :submit.needConfirm').each(function(){
      var btn = $(this);
      
      btn.after('<div class="prompt" style="display:none;"><span class="count"></span><a href="" class="func confirm">Confirm</a> | <a href="" class="func cancel">Cancel</a></div>');
      btn.parent().find('div.prompt a.func').click(function(ev){
        var opt = $(this);
        ev.preventDefault();
        
        if(opt.hasClass('cancel')){
          $this.reset();
        }
        else{
          opt.parent().prev(':submit').click();
        }
      }); 
    });
    
    $('div.gu-view div.toolbar :submit.needConfirm').click(function(){
      return $this.click($(this));
    });
  }
  
  this.click = function(btn){
    this.view = btn.parents('div.gu-view:eq(0)');
    
    if(btn.hasClass('needConfirm')){
      this.reset();
      this.hideUncheck();
      this.countChecked(btn);
      
      btn.next('div.prompt').show();
      btn.removeClass('needConfirm');      
      return false;
    }
    else{
      return true;
    }
  }

  this.countChecked = function(btn)
  {
    var count = this.view.find('table.gu-seltable td.selector :checkbox:checked').length;
    btn.next('div.prompt').find('span.count').text('Count: ' + count);
  }
  
  this.hideUncheck = function(){
    this.view.find('table.gu-seltable td.selector :checkbox').not(':checked').each(function(){
      var tr = $(this).parents('tr:eq(0)');
      tr.hide();
      tr.nextAll('tr').each(function(){
        if(!$(this).attr('merged')){
          return false;
        }
        else{
          $(this).hide();
        }
      });
      
    });
    
    $('html').scrollTop($('table.gu-seltable td.selector :checked').eq(-5).offset().top);
  }
  
  this.reset = function(){
    $('table.gu-seltable tr').fadeIn('fast');
    
    $('div.gu-view div.toolbar :submit').each(function(){
      var btn = $(this);
      var prompt = btn.next('div.prompt');
      if(prompt.css('display') != 'none'){
        btn.addClass('needConfirm');
        prompt.hide();
      }
    });
  }
  
//constructor
  this.init();
}

Drupal.behaviors.GuSelectTableController = function(){
//  init checkbox event
  $('table.gu-seltable tbody td.selector input[type="checkbox"]').change(function(){
    var tr = $(this).parents('tr:eq(0)');
    if($(this).attr('checked')){
      tr.addClass('selected');
      tr.nextAll('tr').each(function(){
        if(!$(this).attr('merged')){
          return false;
        }
        else{
          $(this).addClass('selected');
        }
      });
    }
    else{
      tr.removeClass('selected');
      tr.nextAll('tr').each(function(){
        if(!$(this).attr('merged')){
          return false;
        }
        else{
          $(this).removeClass('selected');
        }
      });
    }
  });
  
  $('table th.selector input[type="checkbox"]').change(function(){
    var Chk = new GuSelTableCheck(this);
    Chk.toggleAll();
  });

  $('select.GuSelectTable.filter').change(function(){  
    var Filter = new GuSelTableFilter(this);
    Filter.check();
  });

  var prompt = new GuSelTablePrompt();
}