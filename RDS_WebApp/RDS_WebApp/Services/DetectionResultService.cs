namespace RDS_WebApp.Services
{
    public class DetectionResultService
    {
        public string?  AnnotatedImageBase64 { get; private set; }
        public double   TargetDeltaAz        { get; private set; }
        public double   TargetDeltaEl        { get; private set; }
        public bool     HasTarget            { get; private set; }
        public double   TargetSvgX           { get; private set; } = 530;
        public double   TargetSvgY           { get; private set; } = 225;

        public event Action? OnTargetDetected;

        public void SetTarget(string imageBase64, double deltaAz, double deltaEl, double svgX = 530, double svgY = 225)
        {
            AnnotatedImageBase64 = imageBase64;
            TargetDeltaAz        = deltaAz;
            TargetDeltaEl        = deltaEl;
            TargetSvgX           = svgX;
            TargetSvgY           = svgY;
            HasTarget            = true;
            OnTargetDetected?.Invoke();
        }

        public void ClearTarget()
        {
            HasTarget            = false;
            AnnotatedImageBase64 = null;
            OnTargetDetected?.Invoke();
        }
    }
}
