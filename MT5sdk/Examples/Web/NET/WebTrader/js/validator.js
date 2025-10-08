(function() {
  var iteration = 0;
  var msgbox;

  function prevNode(node) {
    var o = node.previousSibling;
    while (o && o.nodeType != 1) o = o.previousSibling;
    return o;
  }

  function trim(s) {
    return s.replace(/(^[ \r\n\t]+|[ \r\n\t]+$)/g, "");
  }

  function NotEmpty(input) {
    var s;

    if (!(s = input.value) || (s.replace(/( |\t|\n|\r|\f|\v)/g, '') == ''))
      return false;
    return true;
  }

  function IsNumeric(input) {
    var s;
    if (!(s = input.value) || !(/^[0-9]+(\.|,)?[0-9]*$/.test(s))) {
      return false;
    }
    return true;
  }

  function IsEmail(input) {
    var s;
    if ((s = input.value) && (!/^\w+([-+.'']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*$/i.test(s) || s.length >= 255))
      return false;

    return true;
  }

  function IsEqual(first, second, ignoreCase) {
    var a = first.value, b = second.value;
    if (ignoreCase) {
      a = a.toLowerCase();
      b = b.toLowerCase();
    }
    return (a == b);
  }

  function MatchRegular(input, expression, flags) {
    var r = new RegExp(expression, flags);
    return r.test(input.value);
  }



  function IsUrl(input) {
    if (!input.value) return true;
    return /^((http|https):\/\/)?([a-z0-9\-]+\.)+[a-z]+(:\d+)?(\/[\w-_ .\/%]*)?(\/)?(\?[\w=%&]+)?$/i.test(input.value);
  }

  function IsRadio(inputs) {
    for (i = 0, sz = inputs.length; i < sz; i++)
      if (inputs[i].checked) return true;
    return false;
  }

  function AppendMessage(anchor, message) {
    var mode = 1;
    var nxt = prevNode(anchor);
    //---
    if (msgbox) {
      msgbox.style.display = 'block';
      return true;
    }
    //---
    if (!nxt || nxt.className != 'field-validation-error' || nxt.nodeName != 'SPAN') {
      var s = document.createElement('span');
      s.innerHTML = message;
      s.className = 'field-validation-error';
      anchor.parentNode.insertBefore(s, anchor);
      anchor.parentNode.insertBefore(document.createTextNode("\n"), s);
    }
    else
      nxt.innerHTML = message;
  }

  function ClearMessage(anchor) {
    var nxt = prevNode(anchor);
    if (nxt && nxt.className == 'field-validation-error')
      nxt.innerHTML = "";
  }


  window.Validate = function(form) {
    if (document.getElementById('validate_message')) {
      msgbox = document.getElementById('validate_message');
      msgbox.style.display = 'none';
    }
    if (!window.V) {
      var s = "Error validator";
      if (window.console) console.warn(s);
      else alert(s);
      return false;
    }
    var result = true, y, u, clr = {};
    ++iteration;
    for (var i = 0, l = V.length; i < l; ++i) {
      var valid = true;
      if (u = form.elements[(y = V[i])[0]]) {
        if (u.style && u.style.display == 'none') continue;
        var anchor = document.getElementById('validate_' + y[0]);
        if (u.type == 'text' || u.nodeName == 'TEXTAREA') u.value = trim(u.value);
        switch (y[1]) {
          case 1:
            valid = NotEmpty(u);
            break;
          case 2:
            var len;
            if (y[3] !== null) {
              len = parseInt(y[3])
              var maxlen;
              (y[4] && (maxlen = y[4]));
              valid = u.value.length >= len && (!maxlen || u.value.length <= maxlen);
            }
            break;
          case 3:
            valid = IsNumeric(u);
            break;
          case 4:
            valid = IsEmail(u);
            break;
          case 5:
            valid = IsEqual(u, form.elements[y[3]], y[4]);
            if (!valid) {
              var u2;
              if (clr[y[3]]) {
                var a = document.getElementById('validate_' + y[3]);
                ClearMessage(a);
                clr[y[3]] = false;
              }

              if (!(u2 = form.elements[y[3]]).className)
                u2.className = "field-validation-error";
              else
                if (u2.className.indexOf('field-validation-error') == -1) {
                u2.className += " field-validation-error";

              }
            }
            break;
          case 6:
            valid = MatchRegular(u, y[3], y[4]);
            break;
          case 8:
            valid = IsUrl(u);
            break;
          case 9:
            valid = IsRadio(u);
            break;
          case 10:
            try {
              valid = y[3](u);
            }
            catch (e) {
              if (window.console) console.error(e.message);
            }
            break;
        }
        if (!valid && u.iteration != iteration) {

          setError(anchor, u, y[2]);

          result = false;
          clr[y[0]] = false;
          u.iteration = iteration;
        }
        else if (clr[y[0]] !== false)
          clr[y[0]] = true;
      }
    }

    if (!result) {
      for (var name in clr)
        if (clr[name]) {
        var anchor = document.getElementById('validate_' + name);
        clearError(anchor, form.elements[name]);
      }
      if (window.form_is_not_valid)
        form_is_not_valid(form);
    }
    else if (window.form_is_valid) {
      form_is_valid(form);
    }
    return result;
  }

  function clearError(anchor, node) {
    ClearMessage(anchor);

    function clean(e) {
      var c = e.className.split(' '),
               a = [];
      for (var i = 0, l = c.length; i < l; ++i)
        if (c[i] != 'field-validation-error')
        a.push(c[i]);
      e.className = a.join(' ');
    }

    if (!node.nodeName)
      for (var l = node.length - 1; l >= 0; --l) {
      clean(node[l]);
      clean(node[l].parentNode);
    }
    else {
      clean(node);
      clean(node.parentNode);
    }

  }

  function setError(anchor, node, message) {

    if (node.parentNode.nodeName == 'DIV' && node.parentNode.className.indexOf('inputWrapper') != -1) {
      node.parentNode.className += " field-validation-error";
    }
    else {
      if (!node.className)
        node.className = "field-validation-error";
      else {
        if (node.className.indexOf('field-validation-error') == -1)
          node.className += " field-validation-error";

      }
    }

    if (typeof (message) == "string")
      AppendMessage(anchor, message);
    else
      for (var i = 0, len = message.length; i < len; ++i)
      AppendMessage(anchor, message[i]);
  }

  window.Validate.AppendMessage = AppendMessage;
  window.Validate.ClearMessage = ClearMessage;
  window.Validate.SetValid = function(form, errors) {
    var i, len, error, anchor, name;
    for (i = 0, len = form.elements.length; i < len; ++i) {
      name = form.elements[i].name;
      anchor = document.getElementById('validate_' + name);
      if (!anchor)
        continue;
      if (!(error = errors[name])) {
        clearError(anchor, form.elements[i]);
        continue;
      }
      setError(anchor, form.elements[i], error);
    }
  }
}
)();