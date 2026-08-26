using Microsoft.ML.OnnxRuntime;
using Microsoft.ML.OnnxRuntime.Tensors;
using SixLabors.ImageSharp;
using SixLabors.ImageSharp.Drawing;
using SixLabors.ImageSharp.Drawing.Processing;
using SixLabors.ImageSharp.Processing;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace RDS_WebApp.Services
{
    public record DetectionResult(byte[] AnnotatedImage, List<RectangleF> Boxes, int OriginalWidth, int OriginalHeight);

    public class ImageDetectorService
    {
        // Loaded once and reused — this service is a Singleton (Program.cs), and
        // LiveTrackingService calls DetectPeopleInBytes several times a second;
        // re-loading the ONNX model from disk on every call (the original
        // behavior) made continuous use far too slow.
        private readonly InferenceSession _session;

        public ImageDetectorService(IWebHostEnvironment env)
        {
            var modelPath = System.IO.Path.Combine(env.WebRootPath, "models", "yolov8n.onnx");
            _session = new InferenceSession(modelPath);
        }

        public DetectionResult DetectPeopleInBytes(byte[] inputImageBytes, float contrast)
        {
            using var image = Image.Load<SixLabors.ImageSharp.PixelFormats.Bgr24>(inputImageBytes);
            int originalWidth  = image.Width;
            int originalHeight = image.Height;

            int modelWidth = 640, modelHeight = 640;
            using var resizedImage = image.Clone(x => x
                .Resize(modelWidth, modelHeight)
                .Contrast(contrast)
                .Brightness(1.2f)
            );

            var inputTensor = new DenseTensor<float>(new[] { 1, 3, modelHeight, modelWidth });
            resizedImage.ProcessPixelRows(accessor =>
            {
                for (int y = 0; y < accessor.Height; y++)
                {
                    var row = accessor.GetRowSpan(y);
                    for (int x = 0; x < accessor.Width; x++)
                    {
                        inputTensor[0, 0, y, x] = row[x].R / 255f;
                        inputTensor[0, 1, y, x] = row[x].G / 255f;
                        inputTensor[0, 2, y, x] = row[x].B / 255f;
                    }
                }
            });

            var inputs = new List<NamedOnnxValue> { NamedOnnxValue.CreateFromTensor("images", inputTensor) };
            using var results      = _session.Run(inputs);
            var outputTensor       = results.First().AsTensor<float>();

            float xFactor = (float)originalWidth  / modelWidth;
            float yFactor = (float)originalHeight / modelHeight;

            var boxes       = new List<RectangleF>();
            var confidences = new List<float>();

            int numCandidates = outputTensor.Dimensions[2];
            for (int c = 0; c < numCandidates; c++)
            {
                float confidence = outputTensor[0, 4, c];
                if (confidence >= 0.45f)
                {
                    float cx = outputTensor[0, 0, c];
                    float cy = outputTensor[0, 1, c];
                    float w  = outputTensor[0, 2, c];
                    float h  = outputTensor[0, 3, c];

                    boxes.Add(new RectangleF(
                        (cx - w / 2f) * xFactor,
                        (cy - h / 2f) * yFactor,
                        w * xFactor,
                        h * yFactor));
                    confidences.Add(confidence);
                }
            }

            var finalIndices = ApplyNMS(boxes, confidences, 0.45f);
            var finalBoxes   = finalIndices.Select(i => boxes[i]).ToList();
            var boxColor = Color.ParseHex("e0f0f0");
           
            float strokeThickness = 1.0f;
            float length = 15f;
            image.Mutate(ctx =>
            {
                foreach (int index in finalIndices)
                {
                    var box = boxes[index];

                    float left = box.Left;
                    float right = box.Right;
                    float top = box.Top-1;
                    float bottom = box.Bottom;

                    // בניית ארבע הפינות באמצעות Path המורכב מ-LinearLineSegment  

                    // 1. פינה שמאלית עליונה ┌
                    var topLeftPath = new SixLabors.ImageSharp.Drawing.Path(new LinearLineSegment(
                        new PointF(left + length, top),
                        new PointF(left, top),
                        new PointF(left, top + length)
                    ));

                    // 2. פינה ימנית עליונה ┐
                    var topRightPath = new SixLabors.ImageSharp.Drawing.Path(new LinearLineSegment(
                        new PointF(right - length, top),
                        new PointF(right, top),
                        new PointF(right, top + length)
                    ));

                    // 3. פינה שמאלית תחתונה └
                    var bottomLeftPath = new SixLabors.ImageSharp.Drawing.Path(new LinearLineSegment(
                        new PointF(left, bottom - length),
                        new PointF(left, bottom),
                        new PointF(left + length, bottom)
                    ));

                    // 4. פינה ימנית תחתונה ┘
                    var bottomRightPath = new SixLabors.ImageSharp.Drawing.Path(new LinearLineSegment(
                        new PointF(right, bottom - length),
                        new PointF(right, bottom),
                        new PointF(right - length, bottom)
                    ));

                    // ציור הפינות על התמונה
                    ctx.Draw(boxColor, strokeThickness, topLeftPath);
                    ctx.Draw(boxColor, strokeThickness, topRightPath);
                    ctx.Draw(boxColor, strokeThickness, bottomLeftPath);
                    ctx.Draw(boxColor, strokeThickness, bottomRightPath);

                    // TEMP DEBUG (2026-08-12) — bright red dot baked at the exact
                    // pixel center used for LiveTrackingService's svgX/svgY math,
                    // so the crosshair-alignment bug can be checked visually: does
                    // this dot land where the crosshair renders? Remove once the
                    // bug is confirmed fixed.
                    float cx = left + (right - left) / 2f;
                    float cy = top + (bottom - top) / 2f;
                    ctx.Fill(Color.Red, new EllipsePolygon(new PointF(cx, cy), 6f));
                }
            });
         
            /*       
         image.Mutate(ctx =>
         {
             foreach (int index in finalIndices)
                 ctx.Draw(boxColor, 2.5f, boxes[index]);      
         });
         */
            using var memoryStream = new MemoryStream();
            image.SaveAsPng(memoryStream);
            return new DetectionResult(memoryStream.ToArray(), finalBoxes, originalWidth, originalHeight);
            
        }

        private List<int> ApplyNMS(List<RectangleF> boxes, List<float> confidences, float iouThreshold)
        {
            var indices = confidences
                .Select((c, i) => new { Confidence = c, Index = i })
                .OrderByDescending(x => x.Confidence)
                .Select(x => x.Index)
                .ToList();

            var selected = new List<int>();
            while (indices.Count > 0)
            {
                int current = indices[0];
                selected.Add(current);
                indices.RemoveAt(0);
                for (int i = indices.Count - 1; i >= 0; i--)
                {
                    if (CalculateIoU(boxes[current], boxes[indices[i]]) > iouThreshold)
                        indices.RemoveAt(i);
                }
            }
            return selected;
        }

        private static float CalculateIoU(RectangleF a, RectangleF b)
        {
            float left   = System.Math.Max(a.Left,   b.Left);
            float top    = System.Math.Max(a.Top,    b.Top);
            float right  = System.Math.Min(a.Right,  b.Right);
            float bottom = System.Math.Min(a.Bottom, b.Bottom);

            if (right < left || bottom < top) return 0f;

            float intersection = (right - left) * (bottom - top);
            return intersection / (a.Width * a.Height + b.Width * b.Height - intersection);
        }
    }
}
