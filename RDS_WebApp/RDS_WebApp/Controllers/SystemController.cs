/*using Microsoft.AspNetCore.Mvc;
using RDS_WebApp.Services; // חשוב! כדי שהקונטרולר יכיר את הסרוויס
namespace RDS_WebApp.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class SystemController : ControllerBase
    {
        private readonly SystemController _service;
        public SystemController(SystemController service) => _service = service;

        [HttpGet]
        public IActionResult GetStatus() => Ok(_service.CurrentState);
    }

}*/

///////////////////////////////////////////////////////////////////////////////////
using Microsoft.AspNetCore.Mvc;
using RDS_WebApp.Services;

namespace RDS_WebApp.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class SystemController : ControllerBase
    {
        private readonly SystemControlService _service;

        // ה-Constructor מקבל את ה-Service מהמערכת
        public SystemController(SystemControlService service)
        {
            _service = service;
        }
        [HttpGet]
        public IActionResult GetStatus()
        {
            // שגיאה נפוצה: ניסיון לגשת ל-CurrentState ישירות.
            // תיקון: חייבים להוסיף את הקידומת _service.
            var currentState = _service.CurrentState;

            return Ok(new { state = currentState.ToString() });
        }
         // כאן אנחנו משתמשים ב-UpdateState שנמצא בתוך ה-Service
        [HttpPost("inject-fault")]
        public IActionResult InjectFault()
        {
            // שים לב לשימוש ב- _service.
            _service.UpdateState(SystemState.FAULT, "API: Remote Fault Injection triggered");
            return Ok(new { message = "Fault Injected Successfully" });
        }
    }
}
